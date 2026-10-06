#include "dependencyinstaller.h"
#include "fdrunner.h"
#include "filemanagerhelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QMessageBox>
#include <QRegularExpression>
#include <QFont>
#include <QIcon>
#include <QTimer>
#include <QDebug>

static QString stripAnsiCodes(const QString &input)
{
    static const QRegularExpression ansiRegex(QStringLiteral("\x1b\\[[0-9;]*[a-zA-Z]"));
    return QString(input).replace(ansiRegex, QString());
}

DependencyInstallerDialog::PackageManagerType DependencyInstallerDialog::detectPackageManager()
{
    if (!QStandardPaths::findExecutable(QStringLiteral("pacman")).isEmpty()) {
        return PackageManagerType::Pacman;
    }
    if (!QStandardPaths::findExecutable(QStringLiteral("apt-get")).isEmpty()) {
        return PackageManagerType::Apt;
    }
    if (!QStandardPaths::findExecutable(QStringLiteral("dnf")).isEmpty()) {
        return PackageManagerType::Dnf;
    }
    if (!QStandardPaths::findExecutable(QStringLiteral("zypper")).isEmpty()) {
        return PackageManagerType::Zypper;
    }
    return PackageManagerType::Unknown;
}

QString DependencyInstallerDialog::packageManagerName(PackageManagerType type)
{
    switch (type) {
    case PackageManagerType::Pacman:
        return QStringLiteral("Arch Linux / CachyOS / Manjaro (Pacman)");
    case PackageManagerType::Apt:
        return QStringLiteral("Debian / Ubuntu / Linux Mint (APT)");
    case PackageManagerType::Dnf:
        return QStringLiteral("Fedora / Red Hat (DNF)");
    case PackageManagerType::Zypper:
        return QStringLiteral("openSUSE (Zypper)");
    default:
        return QStringLiteral("Gestore pacchetti non riconosciuto");
    }
}

QString DependencyInstallerDialog::requiredPackageName(PackageManagerType type)
{
    switch (type) {
    case PackageManagerType::Pacman:
    case PackageManagerType::Zypper:
        return QStringLiteral("fd");
    case PackageManagerType::Apt:
    case PackageManagerType::Dnf:
        return QStringLiteral("fd-find");
    default:
        return QStringLiteral("fd");
    }
}

DependencyInstallerDialog::DependencyInstallerDialog(QWidget *parent)
    : QDialog(parent),
      m_headerLabel(nullptr),
      m_statusLabel(nullptr),
      m_progressBar(nullptr),
      m_logEdit(nullptr),
      m_installButton(nullptr),
      m_closeButton(nullptr),
      m_process(new QProcess(this)),
      m_pkgType(detectPackageManager()),
      m_installationSuccess(false)
{
    setupUi();

    connect(m_process, &QProcess::readyReadStandardOutput, this, &DependencyInstallerDialog::onProcessReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &DependencyInstallerDialog::onProcessReadyReadStandardError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &DependencyInstallerDialog::onProcessFinished);
    connect(m_process, &QProcess::errorOccurred, this, &DependencyInstallerDialog::onProcessErrorOccurred);

    // Avvia automaticamente l'installazione poco dopo l'apertura del dialogo
    QTimer::singleShot(250, this, &DependencyInstallerDialog::startInstallation);
}

DependencyInstallerDialog::~DependencyInstallerDialog()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->disconnect(this);
        m_process->kill();
        m_process->waitForFinished(500);
    }
}

void DependencyInstallerDialog::setupUi()
{
    setWindowTitle(tr("FDg - Installazione Motore di Ricerca fd"));
    setWindowIcon(QIcon(QStringLiteral(":/icons/appicon.png")));
    resize(580, 420);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(14, 14, 14, 14);

    // Titolo principale
    m_headerLabel = new QLabel(this);
    QFont titleFont = m_headerLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 2);
    titleFont.setBold(true);
    m_headerLabel->setFont(titleFont);
    m_headerLabel->setText(tr("Installazione del pacchetto '%1'").arg(requiredPackageName(m_pkgType)));
    mainLayout->addWidget(m_headerLabel);

    // Dettaglio sistema
    QLabel *systemLabel = new QLabel(tr("Sistema rilevato: <b>%1</b>").arg(packageManagerName(m_pkgType)), this);
    mainLayout->addWidget(systemLabel);

    // Etichetta stato operazione
    m_statusLabel = new QLabel(tr("Preparazione dell'installazione in corso..."), this);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #2980b9; font-weight: bold;"));
    mainLayout->addWidget(m_statusLabel);

    // Barra di avanzamento
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0); // Modalità continua
    m_progressBar->setTextVisible(false);
    mainLayout->addWidget(m_progressBar);

    // Area di visualizzazione log in tempo reale
    QLabel *logTitle = new QLabel(tr("Dettagli operazione e output del terminale:"), this);
    mainLayout->addWidget(logTitle);

    m_logEdit = new QPlainTextEdit(this);
    m_logEdit->setReadOnly(true);
    QFont monoFont(QStringLiteral("Monospace"));
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setPointSize(9);
    m_logEdit->setFont(monoFont);
    m_logEdit->setStyleSheet(QStringLiteral("QPlainTextEdit { background-color: #1e1e1e; color: #d4d4d4; border-radius: 4px; padding: 6px; }"));
    mainLayout->addWidget(m_logEdit, 1);

    // Pulsanti di azione
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

    m_installButton = new QPushButton(tr("Avvia Installazione"), this);
    m_installButton->setEnabled(false);
    buttonsLayout->addWidget(m_installButton);

    m_closeButton = new QPushButton(tr("Chiudi"), this);
    m_closeButton->setEnabled(false);
    buttonsLayout->addWidget(m_closeButton);

    mainLayout->addLayout(buttonsLayout);

    connect(m_installButton, &QPushButton::clicked, this, &DependencyInstallerDialog::startInstallation);
    connect(m_closeButton, &QPushButton::clicked, this, [this]() {
        if (m_installationSuccess) {
            accept();
        } else {
            reject();
        }
    });
}

void DependencyInstallerDialog::appendLog(const QString &text, bool isError)
{
    QString clean = stripAnsiCodes(text);
    if (clean.isEmpty()) return;

    if (isError) {
        m_logEdit->appendHtml(QStringLiteral("<span style='color: #ff6b6b;'>%1</span>").arg(clean.toHtmlEscaped()));
    } else {
        m_logEdit->appendPlainText(clean);
    }
    m_logEdit->ensureCursorVisible();
}

void DependencyInstallerDialog::startInstallation()
{
    m_installButton->setEnabled(false);
    m_closeButton->setEnabled(false);
    m_progressBar->setRange(0, 0);
    m_logEdit->clear();

    // Verifica preliminare: se il motore è già installato, non eseguire alcuna installazione
    QString existingPath = FdRunner::getFdExecutableName();
    if (!existingPath.isEmpty()) {
        m_installationSuccess = true;
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(100);
        m_statusLabel->setText(tr("Il motore 'fd' è già installato nel sistema."));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #27ae60; font-weight: bold;"));
        appendLog(tr("Eseguibile 'fd' già presente ed operativo in: %1").arg(existingPath));
        appendLog(tr("Nessuna installazione necessaria: tutte le funzionalità di ricerca sono attive."));
        m_closeButton->setEnabled(true);
        m_closeButton->setFocus();
        return;
    }

    if (m_pkgType == PackageManagerType::Unknown) {
        m_statusLabel->setText(tr("Errore: nessun gestore di pacchetti supportato trovato."));
        appendLog(tr("Impossibile determinare il gestore pacchetti (pacman, apt, dnf, zypper non trovati)."), true);
        appendLog(tr("Installa manualmente il comando 'fd' (o 'fd-find') sul tuo sistema operativo."), true);
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(0);
        m_closeButton->setEnabled(true);
        return;
    }

    appendLog(tr("=== Inizio procedura di installazione ==="));
    appendLog(tr("Gestore pacchetti: %1").arg(packageManagerName(m_pkgType)));
    appendLog(tr("Pacchetto target: %1").arg(requiredPackageName(m_pkgType)));

    // Verifica presenza di pkexec (PolicyKit GUI)
    bool hasPkexec = !QStandardPaths::findExecutable(QStringLiteral("pkexec")).isEmpty();
    if (!hasPkexec) {
        m_statusLabel->setText(tr("Errore: strumento di autorizzazione (pkexec) non trovato."));
        appendLog(tr("Impossibile procedere: è necessario PolicyKit (pkexec) per l'autorizzazione di sistema."), true);
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(0);
        m_closeButton->setEnabled(true);
        return;
    }

    appendLog(tr("Richiesta di autorizzazione di sistema tramite PolicyKit (pkexec)..."));
    m_statusLabel->setText(tr("In attesa di autorizzazione di sistema..."));
    QString program = QStringLiteral("pkexec");
    QStringList args;

    switch (m_pkgType) {
    case PackageManagerType::Pacman:
        args << QStringLiteral("pacman") << QStringLiteral("-Sy") << QStringLiteral("--noconfirm") << QStringLiteral("fd");
        break;
    case PackageManagerType::Apt:
        args << QStringLiteral("sh") << QStringLiteral("-c")
             << QStringLiteral("DEBIAN_FRONTEND=noninteractive apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y fd-find");
        break;
    case PackageManagerType::Dnf:
        args << QStringLiteral("dnf") << QStringLiteral("install") << QStringLiteral("-y") << QStringLiteral("fd-find");
        break;
    case PackageManagerType::Zypper:
        args << QStringLiteral("zypper") << QStringLiteral("--non-interactive") << QStringLiteral("in") << QStringLiteral("fd");
        break;
    default:
        break;
    }

    m_statusLabel->setText(tr("Download e installazione in corso..."));
    appendLog(tr("Esecuzione comando: %1 %2").arg(program, args.join(QStringLiteral(" "))));

    // Isolamento dell'ambiente di esecuzione: epura LD_LIBRARY_PATH dell'AppImage
    m_process->setProcessEnvironment(FileManagerHelper::cleanEnvironment());
    m_process->start(program, args);
}

void DependencyInstallerDialog::onProcessReadyReadStandardOutput()
{
    QByteArray data = m_process->readAllStandardOutput();
    QString text = QString::fromLocal8Bit(data);
    appendLog(text);
}

void DependencyInstallerDialog::onProcessReadyReadStandardError()
{
    QByteArray data = m_process->readAllStandardError();
    QString text = QString::fromLocal8Bit(data);
    appendLog(text);
}

void DependencyInstallerDialog::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

    if (exitCode == 0) {
        // Verifica se l'eseguibile è ora rilevato correttamente
        QString installedPath = FdRunner::getFdExecutableName();
        if (!installedPath.isEmpty()) {
            m_installationSuccess = true;
            m_progressBar->setRange(0, 100);
            m_progressBar->setValue(100);
            m_statusLabel->setText(tr("Installazione completata con successo!"));
            m_statusLabel->setStyleSheet(QStringLiteral("color: #27ae60; font-weight: bold;"));

            appendLog(tr("\n=== Operazione conclusa con successo! ==="));
            appendLog(tr("Eseguibile 'fd' confermato in: %1").arg(installedPath));

            QMessageBox::information(
                this,
                tr("Operazione conclusa"),
                tr("Il motore di ricerca 'fd' è stato installato ed è ora pienamente operativo sul tuo sistema!\n\n"
                   "Tutte le funzionalità di ricerca di FDg sono pronte all'uso.")
            );

            m_closeButton->setEnabled(true);
            m_closeButton->setFocus();
            return;
        } else {
            appendLog(tr("Il comando di installazione ha terminato con successo, ma 'fd'/'fdfind' non è ancora nel PATH."), true);
        }
    } else if (exitCode == 126) {
        m_statusLabel->setText(tr("Autorizzazione annullata."));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #e67e22; font-weight: bold;"));
        appendLog(tr("Operazione interrotta: autorizzazione annullata."), true);
    } else {
        m_statusLabel->setText(tr("Installazione fallita (codice uscita: %1)").arg(exitCode));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #c0392b; font-weight: bold;"));
        appendLog(tr("Il comando di installazione si è interrotto con codice di errore: %1").arg(exitCode), true);
    }

    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_installButton->setText(tr("Riprova"));
    m_installButton->setEnabled(true);
    m_closeButton->setEnabled(true);
}

void DependencyInstallerDialog::onProcessErrorOccurred(QProcess::ProcessError error)
{
    QString msg;
    switch (error) {
    case QProcess::FailedToStart:
        msg = tr("Impossibile avviare il processo di installazione.");
        break;
    case QProcess::Crashed:
        msg = tr("Il processo di installazione è terminato in modo anomalo.");
        break;
    case QProcess::Timedout:
        msg = tr("Timeout durante l'esecuzione dell'installazione.");
        break;
    default:
        msg = tr("Errore non specificato durante l'esecuzione del processo.");
        break;
    }

    appendLog(msg, true);
    m_statusLabel->setText(msg);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #c0392b; font-weight: bold;"));
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_installButton->setText(tr("Riprova"));
    m_installButton->setEnabled(true);
    m_closeButton->setEnabled(true);
}
