#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QTreeWidgetItem>
#include <QElapsedTimer>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class FdRunner;

/**
 * @brief Elemento personalizzato per la visualizzazione dei risultati nella lista/albero.
 *
 * Mantiene il percorso completo del file e l'indice temporale di inserimento
 * per preservare l'ordine di coerenza originario restituito da fd.
 */
class SearchResultItem : public QTreeWidgetItem
{
public:
    SearchResultItem(const QString &fileName, const QString &dirPath, const QString &fullPath, int discoveryOrder);

    QString fullPath() const { return m_fullPath; }
    QString fileName() const { return text(0); }
    QString dirPath() const { return text(1); }
    int discoveryOrder() const { return m_discoveryOrder; }

    // Ordinamento naturale (A-Z, 0-9) rispettando la colonna cliccata dall'utente
    bool operator<(const QTreeWidgetItem &other) const override;

private:
    QString m_fullPath;
    int m_discoveryOrder;
};

/**
 * @brief Finestra principale dell'applicazione FDg.
 *
 * Gestisce l'interfaccia utente (separata nel file mainwindow.ui), il ciclo di vita
 * delle ricerche, l'apertura dei file o delle cartelle con selezione, il cambio dinamico
 * della lingua e le notifiche di stato.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    // Slot per controllo del ciclo di vita della ricerca
    void onSearchButtonClicked();       ///< Avvia la ricerca o esegue il kill immediato (SIGKILL) del processo
    void onSearchQueryReturnPressed();   ///< Validazione dei parametri ed esecuzione del comando fd
    void onBrowseFolderClicked();        ///< Apertura dialogo grafico per selezione cartella
    void onHomeFolderClicked();          ///< Imposta istantaneamente la cartella Home utente (~) come percorso

    // Slot del runner asincrono di fd
    void onSearchStarted();              ///< Notifica avvio scansione (aggiorna pulsante in 'Interrompi')
    void onResultsBatchReady(const QStringList &batch); ///< Inserimento ad alte prestazioni di batch di risultati
    void onSearchFinished(int totalFound, qint64 elapsedMs, bool wasCancelled); ///< Conclusione scansione o arresto forzato
    void onSearchError(const QString &errorMessage); ///< Notifica di errore di esecuzione o binario mancante

    // Slot per interazione ed ergonomia della lista risultati
    void onResultItemClicked(QTreeWidgetItem *item, int column);       ///< Mostra il percorso nella status bar (nessuna apertura)
    void onResultItemDoubleClicked(QTreeWidgetItem *item, int column); ///< Col 0: apre file con app associata; Col 1: mostra in cartella
    void onCustomContextMenuRequested(const QPoint &pos);              ///< Menu contestuale con click destro del mouse

    // Azioni del menu contestuale
    void openSelectedFile();             ///< Apre il file selezionato in ambiente isolato
    void showSelectedFileInFolder();     ///< Seleziona ed evidenzia l'elemento nel file manager senza aprirlo
    void copySelectedFilePath();         ///< Copia il percorso assoluto negli appunti
    void copySelectedFileName();         ///< Copia solo il nome file negli appunti

    // Gestione localizzazione e internazionalizzazione (i18n)
    void onLanguageChanged(int index);   ///< Cambio dinamico a caldo della lingua attiva (it, en, de, es, fr)

    // Informazioni e Guida Utente
    void onAboutClicked();               ///< Finestra di dialogo con crediti (PsyTech76, IA) e versione
    void onAboutQtClicked();             ///< Informazioni ufficiali sulle librerie Qt6
    void onOpenGuideClicked();           ///< Apre la guida HTML integrata nel browser minimale (F1)

    // Nuova ricerca e Opzioni Avanzate
    void onNewSearchTriggered();         ///< Ripristina l'interfaccia e cancella tutti i risultati precedenti
    void onToggleAdvancedOptions();      ///< Mostra o nasconde i parametri aggiuntivi di ricerca

    // Integrazione Desktop (Menu Applicazioni e Barra delle Applicazioni)
    void onToggleDesktopIntegration();   ///< Installa o rimuove il lanciatore .desktop e l'icona di sistema

    // Controllo e Installazione dipendenza 'fd'
    void checkFdDependency();            ///< Verifica all'avvio la presenza del motore 'fd' e chiede eventuale installazione
    void onInstallFdClicked();           ///< Apre la finestra di installazione guidata del motore 'fd'

private:
    void setupUiCustomizations();
    void setupConnections();
    void setupLanguages();
    void setupThemeMenu();
    void loadLanguage(const QString &localeCode);
    void updateUiText();
    void updateAdvancedOptionsVisibility(); ///< Aggiorna stato e freccia (▶/▼) dei parametri aggiuntivi
    void updateDesktopIntegrationAction(); ///< Sincronizza il testo del menu con lo stato del file .desktop

    Ui::MainWindow *ui;                      // Interfaccia generata da Qt Designer (.ui)
    FdRunner *m_runner;                      // Esecutore asincrono del comando fd
    QTranslator m_appTranslator;             // Traduttore dell'applicazione
    QTranslator m_qtTranslator;              // Traduttore dei componenti standard Qt
    QString m_currentLocale;                 // Locale attualmente impostato (it, en, de, es, fr)
    int m_itemsCounter;                      // Contatore elementi correnti
    bool m_optionsExpanded;                  // Flag visibilità parametri aggiuntivi di ricerca

    QMenu *m_themeMenu = nullptr;            // Sottomenu selezione tema
    QAction *m_actThemeLight = nullptr;      // Azione tema chiaro (Fusion predefinito)
    QAction *m_actThemeDark = nullptr;       // Azione tema scuro (Fusion dark)
    QAction *m_actThemeSystem = nullptr;     // Azione tema predefinito di sistema
};

#endif // MAINWINDOW_H
