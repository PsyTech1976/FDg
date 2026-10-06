#ifndef FDRUNNER_H
#define FDRUNNER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QElapsedTimer>
#include <QTimer>

/**
 * @brief Tipologia di elementi da cercare con fd.
 */
enum class FdSearchType {
    All,        // Tutti gli elementi (file, directory, symlink)
    FilesOnly,  // Solo file regolari (-t f)
    FoldersOnly // Solo cartelle (-t d)
};

/**
 * @brief Parametri di configurazione per la ricerca tramite il comando fd.
 */
struct FdSearchParams {
    QString query;                  // Voce o pattern da cercare
    QString directory;              // Cartella di partenza
    bool includeHidden = false;     // Includere file nascosti (-H)
    bool caseSensitive = false;     // Distinguere maiuscole e minuscole (-s)
    bool followSymlinks = false;    // Seguire i collegamenti simbolici (-L)
    bool useWildcard = true;        // Supporto caratteri jolly stile MS-DOS (* e ?) tramite --glob
    FdSearchType searchType = FdSearchType::All; // Filtro per tipo di elemento
};

/**
 * @brief Gestore asincrono del processo fd (fdfind) su Linux.
 *
 * Esegue il comando in un processo separato, legge i percorsi trovati in streaming,
 * accumula i risultati in batch temporizzati per garantire la massima reattività
 * dell'interfaccia grafica Qt, ed emette segnali di stato ed errore.
 *
 * Gestisce in modo completamente sicuro il ciclo di vita dei processi, ricreando
 * l'istanza QProcess ad ogni nuova ricerca per scongiurare errori di riavvio
 * o segnali spuri derivanti da processi interrotti in precedenza.
 */
class FdRunner : public QObject
{
    Q_OBJECT

public:
    explicit FdRunner(QObject *parent = nullptr);
    ~FdRunner() override;

    /**
     * @brief Verifica se l'eseguibile fd (o fdfind) è disponibile nel sistema.
     * @return true se disponibile, false altrimenti.
     */
    static bool isFdAvailable();

    /**
     * @brief Restituisce il nome o percorso dell'eseguibile fd trovato nel PATH.
     * Supporta nativamente sia 'fd' (Arch, Fedora) che 'fdfind' (Debian, Ubuntu).
     */
    static QString getFdExecutableName();

    /**
     * @brief Avvia una nuova ricerca con i parametri specificati.
     * @param params Parametri di ricerca.
     * @return true se il processo è stato avviato correttamente, false altrimenti.
     */
    bool startSearch(const FdSearchParams &params);

    /**
     * @brief Interrompe immediatamente e forzatamente la ricerca in corso.
     *
     * Invia SIGKILL al processo fd, disconnette tutti i segnali,
     * svuota tutti i buffer residui in memoria e notifica la conclusione.
     */
    void stopSearch();

    /**
     * @brief Indica se una ricerca è attualmente in esecuzione.
     */
    bool isSearching() const;

signals:
    /**
     * @brief Emesso quando la ricerca ha inizio.
     */
    void searchStarted();

    /**
     * @brief Emesso con un batch di percorsi trovati da aggiungere all'interfaccia.
     * @param batch Lista di percorsi assoluti dei file/cartelle trovati.
     */
    void resultsBatchReady(const QStringList &batch);

    /**
     * @brief Emesso quando la ricerca termina (naturalmente o per interruzione).
     * @param totalFound Numero totale di risultati trovati.
     * @param elapsedMs Tempo impiegato in millisecondi.
     * @param wasCancelled true se interrotta dall'utente, false se terminata regolarmente.
     */
    void searchFinished(int totalFound, qint64 elapsedMs, bool wasCancelled);

    /**
     * @brief Emesso in caso di errore nell'esecuzione del comando fd.
     * @param errorString Descrizione dell'errore.
     */
    void searchError(const QString &errorString);

private slots:
    void onReadyReadStandardOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void flushBuffer();

private:
    void cleanupProcess();          // Arresta e distrugge in sicurezza il processo corrente

    QProcess *m_process;            // Processo Qt per eseguire fd (allocato dinamicamente)
    QElapsedTimer m_timer;          // Cronometro per misurare il tempo di ricerca
    QTimer *m_batchTimer;           // Timer per il flush periodico dei risultati
    QStringList m_outputBuffer;     // Buffer temporaneo per raggruppare i percorsi
    QByteArray m_incompleteLine;    // Buffer per gestire linee parziali nello stream
    int m_totalResultsCount;        // Conteggio totale elementi trovati
    bool m_wasUserCancelled;        // Flag di interruzione richiesta dall'utente
    qint64 m_currentPid;            // PID del processo di ricerca corrente per kill immediato
};

#endif // FDRUNNER_H
