#include "fdrunner.h"

#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <signal.h>

FdRunner::FdRunner(QObject *parent)
    : QObject(parent),
      m_process(nullptr),
      m_batchTimer(new QTimer(this)),
      m_totalResultsCount(0),
      m_wasUserCancelled(false),
      m_currentPid(0)
{
    // Timer per svuotare il buffer periodicamente ogni 40ms
    m_batchTimer->setInterval(40);
    connect(m_batchTimer, &QTimer::timeout, this, &FdRunner::flushBuffer);
}

FdRunner::~FdRunner()
{
    cleanupProcess();
}

bool FdRunner::isFdAvailable()
{
    return !getFdExecutableName().isEmpty();
}

QString FdRunner::getFdExecutableName()
{
    // =========================================================================
    // COMPATIBILITÀ MULTI-DISTRIBUZIONE (ARCH LINUX / DEBIAN / UBUNTU)
    // =========================================================================
    // 1. Su distribuzioni basate su Arch Linux (Arch, CachyOS, Manjaro, EndeavourOS)
    //    e Fedora, l'eseguibile ufficiale è denominato 'fd'.
    // 2. Su distribuzioni basate su Debian (Debian 12/13, Ubuntu, Linux Mint, Pop!_OS),
    //    il pacchetto ufficiale è 'fd-find' e l'eseguibile è installato come 'fdfind'
    //    per prevenire collisioni con un vecchio pacchetto di formattazione floppy.
    // 3. Permettiamo inoltre un override manuale tramite la variabile d'ambiente FD_PATH.

    // Controllo eventuale variabile d'ambiente personalizzata
    QString customPath = qEnvironmentVariable("FD_PATH");
    if (!customPath.isEmpty() && QFileInfo::exists(customPath)) {
        return customPath;
    }

    // Ricerca standard nel PATH per 'fd' (Arch, Fedora, openSUSE)
    QString fdPath = QStandardPaths::findExecutable(QStringLiteral("fd"));
    if (!fdPath.isEmpty()) {
        return fdPath;
    }

    // Ricerca standard nel PATH per 'fdfind' (Debian, Ubuntu, Mint)
    QString fdfindPath = QStandardPaths::findExecutable(QStringLiteral("fdfind"));
    if (!fdfindPath.isEmpty()) {
        return fdfindPath;
    }

    // Ulteriore controllo in percorsi convenzionali (es. /usr/bin/fdfind, ~/.cargo/bin/fd)
    QStringList fallbackCandidates = {
        QStringLiteral("/usr/bin/fdfind"),
        QStringLiteral("/usr/bin/fd"),
        QStringLiteral("/usr/local/bin/fd"),
        QStringLiteral("/usr/local/bin/fdfind"),
        QDir::homePath() + QStringLiteral("/.cargo/bin/fd")
    };

    for (const QString &candidate : fallbackCandidates) {
        if (QFileInfo::exists(candidate) && QFileInfo(candidate).isExecutable()) {
            return candidate;
        }
    }

    return QString();
}

bool FdRunner::isSearching() const
{
    return m_process != nullptr && m_process->state() != QProcess::NotRunning;
}

void FdRunner::cleanupProcess()
{
    if (m_process) {
        // Disconnetti IMMEDIATAMENTE tutti i segnali del processo da questo runner
        // Questo impedisce che errori o terminazioni asincrone vengano notificate alla UI
        m_process->disconnect(this);

        qint64 pid = m_currentPid > 0 ? m_currentPid : m_process->processId();

        // =====================================================================
        // CHIUSURA FORZATA IMMEDIATA DEL PROCESSO (KILL APP / SIGKILL)
        // =====================================================================
        // NOTA DI SICUREZZA PER IL SISTEMA OPERATIVO E I DOCUMENTI:
        // L'utility 'fd' ('fdfind') opera esclusivamente in sola lettura (read-only),
        // scansionando directory e attributi dei file senza MAI effettuare scritture,
        // modifiche o cancellazioni sul filesystem.
        // L'invio forzato di SIGKILL al processo e al suo eventuale process group
        // è pertanto sicuro al 100%, non comporta alcun rischio di corruzione dati
        // o inconsistenze dell'OS, e consente di rilasciare istantaneamente
        // il tempo di calcolo della CPU, la memoria RAM e i descrittori di I/O.
        if (pid > 0) {
#if defined(Q_OS_UNIX)
            ::kill(-static_cast<pid_t>(pid), SIGKILL); // Termina l'intero gruppo di processi
            ::kill(static_cast<pid_t>(pid), SIGKILL);  // Termina il processo principale
#endif
        }

        m_process->kill();
        m_process->close();
        m_process->deleteLater();
        m_process = nullptr;
        m_currentPid = 0;
    }
}

bool FdRunner::startSearch(const FdSearchParams &params)
{
    // =========================================================================
    // RISOLUZIONE DEL PROBLEMA DI ERRORE ALL'AVVIO DI UNA NUOVA RICERCA
    // =========================================================================
    // Se c'era una ricerca precedente o interrotta, la ripuliamo e disconnettiamo
    // completamente. Creiamo poi una NUOVA istanza fresca di QProcess per evitare
    // l'errore "Process is already running" o stati sporchi nei descrittori di file.
    stopSearch();

    QString program = getFdExecutableName();
    if (program.isEmpty()) {
        emit searchError(tr("Il comando 'fd' (o 'fdfind') non è stato trovato nel sistema. Installalo con il gestore pacchetti."));
        return false;
    }

    // Preparazione degli argomenti da passare al comando fd
    QStringList args;

    // Disabilita colori ANSI per parsing pulito del testo
    args << QStringLiteral("--color=never");

    // Restituisce sempre percorsi assoluti
    args << QStringLiteral("--absolute-path");

    // File nascosti
    if (params.includeHidden) {
        args << QStringLiteral("--hidden");
    }

    // Rispetto di maiuscole/minuscole
    if (params.caseSensitive) {
        args << QStringLiteral("--case-sensitive");
    } else {
        args << QStringLiteral("--ignore-case");
    }

    // Seguire collegamenti simbolici
    if (params.followSymlinks) {
        args << QStringLiteral("--follow");
    }

    // Filtro per tipologia di file
    switch (params.searchType) {
    case FdSearchType::FilesOnly:
        args << QStringLiteral("-t") << QStringLiteral("f");
        break;
    case FdSearchType::FoldersOnly:
        args << QStringLiteral("-t") << QStringLiteral("d");
        break;
    case FdSearchType::All:
    default:
        break;
    }

    // =========================================================================
    // GESTIONE AVANZATA DEI CARATTERI JOLLY STILE MS-DOS (* e ?)
    // =========================================================================
    // Su fd, l'opzione --glob (-g) consente di utilizzare i caratteri jolly tipici
    // di MS-DOS e della shell (* per qualsiasi stringa, ? per un singolo carattere).
    // Per consentire ricerche flessibili come "nome*.jpg", se il pattern non inizia
    // già con '*' o '/', aggiungiamo un asterisco iniziale "*nome*.jpg", così da
    // individuare sia file che iniziano con "nome" (es. "nome1.jpg") sia file che
    // contengono prefissi o percorsi (es. "mio_nome1.jpg", "foto_nome_vacanze.jpg").
    QString rawQuery = params.query.trimmed();
    bool hasWildcardChar = rawQuery.contains('*') || rawQuery.contains('?');

    if (rawQuery.isEmpty()) {
        args << QStringLiteral(".");
    } else if (params.useWildcard || hasWildcardChar) {
        args << QStringLiteral("--glob");

        QString globPattern = rawQuery;
        // Se non inizia già con asterisco o percorsi assoluti/ancoraggi,
        // anteponiamo l'asterisco per matching parziale sia iniziale che interno
        if (!globPattern.startsWith('*') && !globPattern.startsWith('/') && !globPattern.startsWith('^')) {
            globPattern = QStringLiteral("*%1").arg(rawQuery);
        }

        // Se non contiene caratteri jolly espliciti (es. una semplice parola "documento"),
        // aggiungiamo anche un asterisco finale "*documento*"
        if (!hasWildcardChar && !globPattern.endsWith('*')) {
            globPattern.append('*');
        }

        args << globPattern;
    } else {
        args << rawQuery;
    }

    // Cartella di partenza per la scansione
    QString searchDir = params.directory.trimmed();
    if (searchDir.isEmpty() || !QDir(searchDir).exists()) {
        searchDir = QDir::homePath();
    }
    args << searchDir;

    // Reset completo dello stato interno
    m_outputBuffer.clear();
    m_incompleteLine.clear();
    m_totalResultsCount = 0;
    m_wasUserCancelled = false;

    // Alloca una NUOVA istanza fresca di QProcess
    m_process = new QProcess(this);
#if defined(Q_OS_UNIX)
    // Assegna il processo al proprio process group indipendente per kill istantaneo
    m_process->setChildProcessModifier([]() {
        setpgid(0, 0);
    });
#endif
    connect(m_process, &QProcess::readyReadStandardOutput, this, &FdRunner::onReadyReadStandardOutput);
    connect(m_process, &QProcess::finished, this, &FdRunner::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &FdRunner::onProcessError);

    // Avvio cronometro e processo
    m_timer.start();
    m_batchTimer->start();

    m_process->start(program, args);
    if (!m_process->waitForStarted(2000)) {
        m_batchTimer->stop();
        QString err = m_process->errorString();
        cleanupProcess();
        emit searchError(tr("Impossibile avviare il processo: %1").arg(err));
        return false;
    }

    m_currentPid = m_process->processId();

    emit searchStarted();
    return true;
}

void FdRunner::stopSearch()
{
    // =========================================================================
    // ARRESTO IMMEDIATO E PULIZIA COMPLETA
    // =========================================================================
    m_wasUserCancelled = true;
    m_batchTimer->stop();

    // Svuota immediatamente tutti i buffer in memoria
    m_outputBuffer.clear();
    m_incompleteLine.clear();

    // Arresta, disconnette e distrugge il processo corrente
    cleanupProcess();

    // Emette subito il segnale di conclusione con flag wasCancelled = true
    qint64 elapsedMs = m_timer.elapsed();
    emit searchFinished(m_totalResultsCount, elapsedMs, true);
}

void FdRunner::onReadyReadStandardOutput()
{
    if (m_wasUserCancelled || !m_process) {
        return;
    }

    QByteArray data = m_process->readAllStandardOutput();
    if (data.isEmpty()) {
        return;
    }

    m_incompleteLine.append(data);

    int newlineIndex = 0;
    while ((newlineIndex = m_incompleteLine.indexOf('\n')) != -1) {
        if (m_wasUserCancelled) {
            m_incompleteLine.clear();
            return;
        }

        QByteArray lineBytes = m_incompleteLine.left(newlineIndex);
        m_incompleteLine.remove(0, newlineIndex + 1);

        if (lineBytes.endsWith('\r')) {
            lineBytes.chop(1);
        }

        QString filePath = QString::fromUtf8(lineBytes).trimmed();
        if (!filePath.isEmpty()) {
            m_outputBuffer.append(filePath);
            m_totalResultsCount++;
        }
    }

    // Flush immediato se il buffer supera 300 elementi
    if (m_outputBuffer.size() >= 300) {
        flushBuffer();
    }
}

void FdRunner::flushBuffer()
{
    if (m_wasUserCancelled) {
        m_outputBuffer.clear();
        return;
    }

    if (!m_outputBuffer.isEmpty()) {
        QStringList batch = m_outputBuffer;
        m_outputBuffer.clear();
        emit resultsBatchReady(batch);
    }
}

void FdRunner::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    m_batchTimer->stop();

    // Se interrotta dall'utente, stopSearch() ha già emesso la conclusione
    if (m_wasUserCancelled) {
        return;
    }

    // Svuota gli ultimi elementi rimasti per termine naturale
    if (!m_incompleteLine.isEmpty()) {
        QString lastLine = QString::fromUtf8(m_incompleteLine).trimmed();
        if (!lastLine.isEmpty()) {
            m_outputBuffer.append(lastLine);
            m_totalResultsCount++;
        }
        m_incompleteLine.clear();
    }
    flushBuffer();

    qint64 elapsedMs = m_timer.elapsed();
    emit searchFinished(m_totalResultsCount, elapsedMs, false);
}

void FdRunner::onProcessError(QProcess::ProcessError error)
{
    if (m_wasUserCancelled) {
        return;
    }

    if (error == QProcess::FailedToStart) {
        emit searchError(tr("Errore durante l'avvio del comando fd."));
    } else if (error == QProcess::Crashed) {
        emit searchError(tr("Il processo fd è terminato in modo anomalo."));
    }
}
