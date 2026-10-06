#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fdrunner.h"
#include "filemanagerhelper.h"
#include "guidedialog.h"
#include "desktopintegrator.h"
#include "dependencyinstaller.h"

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QMenu>
#include <QHeaderView>
#include <QApplication>
#include <QLocale>
#include <QKeyEvent>
#include <QSettings>
#include <QTimer>
#include <QProcess>
#include <QDebug>

// =============================================================================
// Implementazione di SearchResultItem (Elemento della lista)
// =============================================================================

SearchResultItem::SearchResultItem(const QString &fileName, const QString &dirPath, const QString &fullPath, int discoveryOrder)
    : QTreeWidgetItem(),
      m_fullPath(fullPath),
      m_discoveryOrder(discoveryOrder)
{
    // Colonna 0: Nome del file
    setText(0, fileName);
    // Colonna 1: Percorso della cartella contenitrice
    setText(1, dirPath);

    // Suggerimento (Tooltip) con il percorso assoluto completo
    setToolTip(0, fullPath);
    setToolTip(1, fullPath);
}

bool SearchResultItem::operator<(const QTreeWidgetItem &other) const
{
    const SearchResultItem *otherItem = dynamic_cast<const SearchResultItem*>(&other);
    if (!otherItem) {
        return QTreeWidgetItem::operator<(other);
    }

    int sortCol = treeWidget() ? treeWidget()->sortColumn() : 0;
    const QString &textA = text(sortCol);
    const QString &textB = otherItem->text(sortCol);

    // Confronto naturale insensibile a maiuscole/minuscole (A-Z, 0-9)
    int cmp = textA.localeAwareCompare(textB);
    if (cmp != 0) {
        return cmp < 0;
    }

    // A parità di testo, preserva l'ordine cronologico di scoperta originario
    return m_discoveryOrder < otherItem->m_discoveryOrder;
}

// =============================================================================
// Implementazione di MainWindow (Finestra principale)
// =============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_runner(new FdRunner(this)),
      m_currentLocale(QStringLiteral("it")),
      m_itemsCounter(0),
      m_optionsExpanded(false)
{
    // Inizializzazione dell'interfaccia grafica creata con Qt Designer (.ui)
    ui->setupUi(this);

    // Configurazione personalizzata dei componenti grafici
    setupUiCustomizations();

    // Connessione dei segnali e degli slot
    setupConnections();

    // Configurazione del selettore delle lingue
    setupLanguages();

    // Carica la lingua italiana di default
    loadLanguage(QStringLiteral("it"));

    // Verifica all'avvio la presenza del motore 'fd'/'fdfind'
    QTimer::singleShot(300, this, &MainWindow::checkFdDependency);
}

MainWindow::~MainWindow()
{
    if (m_runner && m_runner->isSearching()) {
        m_runner->stopSearch();
    }
    delete ui;
}

void MainWindow::setupUiCustomizations()
{
    // Icona della finestra
    setWindowIcon(QIcon(QStringLiteral(":/icons/appicon.png")));

    // Imposta la cartella di partenza predefinita sulla radice del disco "/"
    ui->folderLineEdit->setText(QStringLiteral("/"));

    // Configurazione della lista/albero dei risultati
    ui->resultsTreeWidget->setColumnCount(2);
    ui->resultsTreeWidget->setHeaderLabels(QStringList() << tr("Nome File") << tr("Percorso"));
    ui->resultsTreeWidget->setRootIsDecorated(false);
    ui->resultsTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Gestione colonne e larghezze
    QHeaderView *header = ui->resultsTreeWidget->header();
    header->setSectionResizeMode(0, QHeaderView::Interactive);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->resizeSection(0, 320);
    header->setSectionsClickable(true);
    header->setSortIndicatorShown(true);

    // Inizialmente l'ordinamento automatico è disabilitato per preservare
    // l'ordine di coerenza/scoperta restituito da fd
    ui->resultsTreeWidget->setSortingEnabled(false);

    // Abilita l'ordinamento non appena l'utente clicca su una colonna dell'intestazione
    connect(header, &QHeaderView::sectionClicked, this, [this](int logicalIndex) {
        Q_UNUSED(logicalIndex);
        ui->resultsTreeWidget->setSortingEnabled(true);
    });

    // Barra di avanzamento inizialmente inattiva
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    ui->statusLabel->setText(tr("Pronto per la ricerca"));

    // Requisito: Il pulsante Cerca è inizialmente disabilitato poiché il campo è vuoto
    ui->searchButton->setEnabled(!ui->searchLineEdit->text().trimmed().isEmpty());

    // Configurazione dei parametri aggiuntivi di ricerca collassabili/nascondibili
    QSettings settings(QStringLiteral("PsyTech76"), QStringLiteral("FDg"));
    m_optionsExpanded = settings.value(QStringLiteral("ui/optionsExpanded"), false).toBool();
    ui->toggleOptionsButton->setStyleSheet(QStringLiteral(
        "QToolButton#toggleOptionsButton { font-weight: bold; border: none; background: transparent; padding: 2px 4px; } "
        "QToolButton#toggleOptionsButton:hover { color: #2980b9; }"
    ));
    updateAdvancedOptionsVisibility();

    // Inizializza lo stato della voce di menu per l'integrazione desktop XDG
    updateDesktopIntegrationAction();

    // Focus iniziale sul campo di testo della ricerca
    ui->searchLineEdit->setFocus();
}

void MainWindow::setupConnections()
{
    // Requisito: Aggiornamento dinamico abilitazione pulsante cerca se il campo è vuoto
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (!m_runner->isSearching()) {
            ui->searchButton->setEnabled(!text.trimmed().isEmpty());
        }
    });

    // Click del pulsante Cerca / Interrompi
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);

    // Pressione del tasto Invio nel campo di ricerca
    connect(ui->searchLineEdit, &QLineEdit::returnPressed, this, &MainWindow::onSearchQueryReturnPressed);

    // Pulsante Sfoglia cartella
    connect(ui->browseButton, &QPushButton::clicked, this, &MainWindow::onBrowseFolderClicked);

    // Segnali del runner asincrono di fd
    connect(m_runner, &FdRunner::searchStarted, this, &MainWindow::onSearchStarted);
    connect(m_runner, &FdRunner::resultsBatchReady, this, &MainWindow::onResultsBatchReady);
    connect(m_runner, &FdRunner::searchFinished, this, &MainWindow::onSearchFinished);
    connect(m_runner, &FdRunner::searchError, this, &MainWindow::onSearchError);

    // Click singolo: visualizza il percorso completo nella barra di stato (non apre file)
    connect(ui->resultsTreeWidget, &QTreeWidget::itemClicked, this, &MainWindow::onResultItemClicked);

    // =========================================================================
    // RISOLUZIONE: DOPPIO CLICK ESCLUSIVO (NESSUNA APERTURA CON SINGOLO CLICK)
    // =========================================================================
    // Colleghiamo ESCLUSIVAMENTE itemDoubleClicked. Il segnale Qt 'itemActivated'
    // è stato rimosso poiché su desktop con stile KDE Plasma viene emesso al
    // semplice click singolo, causando aperture indesiderate.
    connect(ui->resultsTreeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onResultItemDoubleClicked);

    // Installiamo un eventFilter per gestire l'apertura tramite tasto Invio da tastiera
    ui->resultsTreeWidget->installEventFilter(this);

    // Menu contestuale (tasto destro)
    connect(ui->resultsTreeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);

    // Selezione lingua dal ComboBox
    connect(ui->languageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onLanguageChanged);

    // Pulsante Home per impostare la cartella utente
    connect(ui->homeFolderButton, &QPushButton::clicked, this, &MainWindow::onHomeFolderClicked);

    // Toggle parametri aggiuntivi di ricerca (mostra/nascondi opzioni e lingua)
    connect(ui->toggleOptionsButton, &QToolButton::clicked, this, &MainWindow::onToggleAdvancedOptions);

    // Azione Nuova Ricerca: cancella testo, disabilita cerca e resetta completamente i risultati precedenti
    connect(ui->actionNuovaRicerca, &QAction::triggered, this, &MainWindow::onNewSearchTriggered);

    // Verifica / Installazione motore di ricerca 'fd'
    connect(ui->actionInstallFd, &QAction::triggered, this, &MainWindow::onInstallFdClicked);

    // Integrazione Desktop (Installa / Rimuovi lanciatore .desktop)
    connect(ui->actionToggleDesktopIntegration, &QAction::triggered, this, &MainWindow::onToggleDesktopIntegration);

    // Azioni menu e pulsanti informazioni e guida
    connect(ui->actionEsci, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionGuida, &QAction::triggered, this, &MainWindow::onOpenGuideClicked);
    connect(ui->guideButton, &QPushButton::clicked, this, &MainWindow::onOpenGuideClicked);
    connect(ui->actionInformazioni, &QAction::triggered, this, &MainWindow::onAboutClicked);
    connect(ui->actionInformazioniQt, &QAction::triggered, this, &MainWindow::onAboutQtClicked);
    connect(ui->aboutButton, &QPushButton::clicked, this, &MainWindow::onAboutClicked);
}

void MainWindow::setupLanguages()
{
    // =========================================================================
    // RISOLUZIONE: VISUALIZZAZIONE CORRETTA DELLE BANDIERE
    // =========================================================================
    // Usiamo icone PNG/SVG esplicite caricate dalle risorse Qt anziché emoji
    // Unicode che su molti ambienti Linux (senza font emoji a colori) appaiono
    // spezzate, quadrate o come lettere di codice paese.
    ui->languageComboBox->blockSignals(true);
    ui->languageComboBox->clear();
    ui->languageComboBox->setIconSize(QSize(20, 15));
    ui->languageComboBox->addItem(QIcon(QStringLiteral(":/icons/flags/it.png")), QStringLiteral("Italiano"), QStringLiteral("it"));
    ui->languageComboBox->addItem(QIcon(QStringLiteral(":/icons/flags/en.png")), QStringLiteral("English"), QStringLiteral("en"));
    ui->languageComboBox->addItem(QIcon(QStringLiteral(":/icons/flags/de.png")), QStringLiteral("Deutsch"), QStringLiteral("de"));
    ui->languageComboBox->addItem(QIcon(QStringLiteral(":/icons/flags/es.png")), QStringLiteral("Español"), QStringLiteral("es"));
    ui->languageComboBox->addItem(QIcon(QStringLiteral(":/icons/flags/fr.png")), QStringLiteral("Français"), QStringLiteral("fr"));
    ui->languageComboBox->setCurrentIndex(0);
    ui->languageComboBox->blockSignals(false);
}

void MainWindow::loadLanguage(const QString &localeCode)
{
    m_currentLocale = localeCode;

    // Rimuove traduttori precedenti
    qApp->removeTranslator(&m_appTranslator);
    qApp->removeTranslator(&m_qtTranslator);

    // Carica traduzione dell'applicazione da risorsa incorporata
    QString qmPath = QStringLiteral(":/translations/fd_frontend_%1.qm").arg(localeCode);
    if (m_appTranslator.load(qmPath)) {
        qApp->installTranslator(&m_appTranslator);
    } else {
        qWarning() << "[MainWindow] Impossibile caricare traduzione:" << qmPath;
    }

    // Carica traduzioni standard di Qt (dialoghi di sistema, bottoni OK/Annulla)
    if (m_qtTranslator.load(QStringLiteral("qtbase_%1").arg(localeCode), QStringLiteral("/usr/share/qt6/translations"))) {
        qApp->installTranslator(&m_qtTranslator);
    }

    // Aggiorna l'interfaccia utente con la nuova lingua selezionata
    ui->retranslateUi(this);
    updateUiText();
}

void MainWindow::updateUiText()
{
    // Re-imposta le etichette delle colonne
    ui->resultsTreeWidget->setHeaderLabels(QStringList() << tr("Nome File") << tr("Percorso"));

    // Voce obbligatoria PsyTech76 & IA (con traduzione multi-lingua)
    ui->attributionLabel->setText(tr("Applicazione realizzata con IA su un'idea di PsyTech76 - Gratuita"));

    // Stato del pulsante di ricerca
    if (m_runner->isSearching()) {
        ui->searchButton->setText(tr("Interrompi"));
        ui->searchButton->setEnabled(true);
    } else {
        ui->searchButton->setText(tr("Cerca"));
        ui->searchButton->setEnabled(!ui->searchLineEdit->text().trimmed().isEmpty());
    }

    // Aggiorna etichetta di stato
    if (m_runner->isSearching()) {
        ui->statusLabel->setText(tr("Ricerca in corso... Trovati: %1").arg(m_itemsCounter));
    } else if (m_itemsCounter > 0) {
        ui->statusLabel->setText(tr("%1 elementi visualizzati").arg(m_itemsCounter));
    } else {
        ui->statusLabel->setText(tr("Pronto per la ricerca"));
    }

    // Aggiorna testo del pulsante parametri aggiuntivi (▶ o ▼)
    QString arrow = m_optionsExpanded ? QString::fromUtf8("▼ ") : QString::fromUtf8("▶ ");
    ui->toggleOptionsButton->setText(arrow + tr("Parametri aggiuntivi di ricerca"));
    ui->toggleOptionsButton->setToolTip(tr("Mostra o nascondi le opzioni avanzate di ricerca e localizzazione"));

    // Aggiorna testo e stato dell'integrazione desktop in base alla lingua attiva
    updateDesktopIntegrationAction();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateUiText();
    }
    QMainWindow::changeEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // Gestione della pressione del tasto Invio sulla lista dei risultati
    if (watched == ui->resultsTreeWidget && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            QTreeWidgetItem *currentItem = ui->resultsTreeWidget->currentItem();
            if (currentItem) {
                // Tasto Invio apre il file associato
                onResultItemDoubleClicked(currentItem, 0);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::onLanguageChanged(int index)
{
    QString langCode = ui->languageComboBox->itemData(index).toString();
    loadLanguage(langCode);
}

void MainWindow::onSearchButtonClicked()
{
    if (m_runner->isSearching()) {
        // =====================================================================
        // ANNULLAMENTO IMMEDIATO (KILL APP / SIGKILL)
        // =====================================================================
        // Chiude forzatamente il processo in background e aggiorna subito la GUI
        m_runner->stopSearch();
        ui->searchButton->setText(tr("Cerca"));
        ui->searchButton->setStyleSheet(QString());
        ui->searchButton->setEnabled(!ui->searchLineEdit->text().trimmed().isEmpty());
        ui->progressBar->setRange(0, 100);
        ui->progressBar->setValue(100);
        ui->statusLabel->setText(tr("Ricerca interrotta: %1 elementi trovati").arg(m_itemsCounter));
    } else {
        // Altrimenti avvia una nuova ricerca
        onSearchQueryReturnPressed();
    }
}

void MainWindow::onSearchQueryReturnPressed()
{
    QString query = ui->searchLineEdit->text().trimmed();

    // =========================================================================
    // REQUISITO: DISABILITA LA RICERCA SE IL CAMPO CERCA È VUOTO
    // =========================================================================
    if (query.isEmpty()) {
        statusBar()->showMessage(tr("Inserisci un termine o un pattern prima di avviare la ricerca."), 3000);
        return;
    }

    QString folder = ui->folderLineEdit->text().trimmed();

    // Di default, se il percorso è vuoto o non valido, impostiamo la root del disco "/"
    if (folder.isEmpty() || !QDir(folder).exists()) {
        folder = QStringLiteral("/");
        ui->folderLineEdit->setText(folder);
    }

    FdSearchParams params;
    params.query = query;
    params.directory = folder;
    params.includeHidden = ui->hiddenCheckBox->isChecked();
    params.caseSensitive = ui->caseSensitiveCheckBox->isChecked();
    params.followSymlinks = ui->symlinksCheckBox->isChecked();
    params.useWildcard = ui->wildcardCheckBox->isChecked(); // Supporto caratteri jolly stile MS-DOS

    int typeIndex = ui->typeComboBox->currentIndex();
    if (typeIndex == 1) {
        params.searchType = FdSearchType::FilesOnly;
    } else if (typeIndex == 2) {
        params.searchType = FdSearchType::FoldersOnly;
    } else {
        params.searchType = FdSearchType::All;
    }

    // Pulisce i risultati precedenti
    ui->resultsTreeWidget->setSortingEnabled(false);
    ui->resultsTreeWidget->clear();
    m_itemsCounter = 0;

    // Avvia la ricerca con fd
    if (!m_runner->startSearch(params)) {
        ui->statusLabel->setText(tr("Errore avvio ricerca"));
    }
}

void MainWindow::onBrowseFolderClicked()
{
    QString current = ui->folderLineEdit->text();
    if (current.isEmpty() || !QDir(current).exists()) {
        current = QStringLiteral("/");
    }

    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Seleziona cartella per la ricerca"),
        current,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!dir.isEmpty()) {
        ui->folderLineEdit->setText(dir);
    }
}

void MainWindow::onSearchStarted()
{
    // Cambia il pulsante in "Interrompi" (sempre abilitato per consentire il kill)
    ui->searchButton->setEnabled(true);
    ui->searchButton->setText(tr("Interrompi"));
    ui->searchButton->setStyleSheet(QStringLiteral("QPushButton { background-color: #d9534f; color: white; font-weight: bold; }"));

    // Barra di avanzamento in modalità indeterminata (animazione continua)
    ui->progressBar->setRange(0, 0);
    ui->statusLabel->setText(tr("Ricerca in corso... Trovati: 0"));
}

void MainWindow::onResultsBatchReady(const QStringList &batch)
{
    // Se la ricerca è stata interrotta, scarta immediatamente eventuali pacchetti residui
    if (!m_runner->isSearching()) {
        return;
    }

    QList<QTreeWidgetItem*> itemsToAdd;
    itemsToAdd.reserve(batch.size());

    for (const QString &fullPath : batch) {
        // =====================================================================
        // RISOLUZIONE BUG: NOME FILE CHE MOSTRA IL PERCORSO INVECE DEL NOME
        // =====================================================================
        // Quando il comando 'fd' individua una cartella (es. "/path/to/Cartella/"),
        // la presenza dello slash finale porta QFileInfo::fileName() a restituire
        // una stringa vuota "". Senza una corretta normalizzazione, il fallback
        // inseriva l'intero percorso assoluto nella colonna "Nome File" e la
        // cartella stessa nella colonna "Percorso".
        // Rimuovendo gli slash finali ridondanti (tranne per la sola root "/"):
        // 1. fileName() estrae regolarmente il solo nome della cartella o del file.
        // 2. absolutePath() restituisce correttamente il percorso della cartella genitrice.
        QString cleanPath = fullPath;
        while (cleanPath.endsWith(QLatin1Char('/')) && cleanPath.length() > 1) {
            cleanPath.chop(1);
        }

        QFileInfo fi(cleanPath);
        QString fileName = fi.fileName();
        if (fileName.isEmpty()) {
            fileName = cleanPath; // Per la sola root "/" o nomi speciali
        }
        QString dirPath = fi.absolutePath();

        SearchResultItem *item = new SearchResultItem(fileName, dirPath, cleanPath, m_itemsCounter++);
        itemsToAdd.append(item);
    }

    // Inserimento a blocchi per massimizzare le prestazioni
    ui->resultsTreeWidget->addTopLevelItems(itemsToAdd);

    // Aggiorna lo stato in tempo reale
    ui->statusLabel->setText(tr("Ricerca in corso... Trovati: %1").arg(m_itemsCounter));
}

void MainWindow::onSearchFinished(int totalFound, qint64 elapsedMs, bool wasCancelled)
{
    // Ripristina il pulsante a "Cerca" e aggiorna lo stato di abilitazione
    ui->searchButton->setText(tr("Cerca"));
    ui->searchButton->setStyleSheet(QString());
    ui->searchButton->setEnabled(!ui->searchLineEdit->text().trimmed().isEmpty());

    // Barra di avanzamento completata al 100%
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(100);

    double seconds = static_cast<double>(elapsedMs) / 1000.0;

    if (wasCancelled) {
        ui->statusLabel->setText(tr("Ricerca interrotta: %1 elementi trovati").arg(totalFound));
        statusBar()->showMessage(tr("Ricerca interrotta dall'utente dopo %1 s").arg(seconds, 0, 'f', 2), 4000);
    } else {
        ui->statusLabel->setText(tr("Completato: %1 elementi in %2 s").arg(totalFound).arg(seconds, 0, 'f', 2));
        statusBar()->showMessage(tr("Ricerca completata: %1 risultati in %2 secondi").arg(totalFound).arg(seconds, 0, 'f', 2), 4000);
    }
}

void MainWindow::onSearchError(const QString &errorMessage)
{
    ui->searchButton->setText(tr("Cerca"));
    ui->searchButton->setStyleSheet(QString());
    ui->searchButton->setEnabled(!ui->searchLineEdit->text().trimmed().isEmpty());

    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    ui->statusLabel->setText(tr("Errore durante la ricerca"));

    QMessageBox::warning(this, tr("Errore Ricerca FD"), errorMessage);
}

// -----------------------------------------------------------------------------
// Gestione dei click e doppi click sui risultati
// -----------------------------------------------------------------------------

void MainWindow::onResultItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    SearchResultItem *resItem = dynamic_cast<SearchResultItem*>(item);
    if (!resItem) return;

    // Mostra il percorso completo nella barra di stato in basso al semplice click
    statusBar()->showMessage(resItem->fullPath(), 3000);
}

void MainWindow::onResultItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    SearchResultItem *resItem = dynamic_cast<SearchResultItem*>(item);
    if (!resItem) return;

    if (column == 0) {
        // =====================================================================
        // REQUISITO: DOPPIO CLICK SULLA COLONNA NOME FILE
        // Apre il file con l'applicazione associata dal desktop di sistema
        // (testi, audio, video, immagini, documenti, pdf, ecc.)
        // =====================================================================
        FileManagerHelper::openFile(resItem->fullPath());
    } else if (column == 1) {
        // =====================================================================
        // REQUISITO: DOPPIO CLICK SULLA COLONNA PERCORSO
        // Apre la cartella nel file manager ed evidenzia/seleziona il file
        // =====================================================================
        FileManagerHelper::showItemInFolder(resItem->fullPath());
    }
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *rawItem = ui->resultsTreeWidget->itemAt(pos);
    SearchResultItem *item = dynamic_cast<SearchResultItem*>(rawItem);
    if (!item) return;

    QMenu menu(this);
    QAction *actOpenFile = menu.addAction(QIcon::fromTheme(QStringLiteral("document-open")), tr("Apri file"));
    QAction *actShowInFolder = menu.addAction(QIcon::fromTheme(QStringLiteral("folder-open")), tr("Mostra nella cartella (seleziona)"));
    menu.addSeparator();
    QAction *actCopyPath = menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("Copia percorso completo"));
    QAction *actCopyName = menu.addAction(tr("Copia nome file"));

    connect(actOpenFile, &QAction::triggered, this, &MainWindow::openSelectedFile);
    connect(actShowInFolder, &QAction::triggered, this, &MainWindow::showSelectedFileInFolder);
    connect(actCopyPath, &QAction::triggered, this, &MainWindow::copySelectedFilePath);
    connect(actCopyName, &QAction::triggered, this, &MainWindow::copySelectedFileName);

    menu.exec(ui->resultsTreeWidget->viewport()->mapToGlobal(pos));
}

void MainWindow::openSelectedFile()
{
    QTreeWidgetItem *current = ui->resultsTreeWidget->currentItem();
    SearchResultItem *item = dynamic_cast<SearchResultItem*>(current);
    if (item) {
        FileManagerHelper::openFile(item->fullPath());
    }
}

void MainWindow::showSelectedFileInFolder()
{
    QTreeWidgetItem *current = ui->resultsTreeWidget->currentItem();
    SearchResultItem *item = dynamic_cast<SearchResultItem*>(current);
    if (item) {
        FileManagerHelper::showItemInFolder(item->fullPath());
    }
}

void MainWindow::copySelectedFilePath()
{
    QTreeWidgetItem *current = ui->resultsTreeWidget->currentItem();
    SearchResultItem *item = dynamic_cast<SearchResultItem*>(current);
    if (item) {
        QApplication::clipboard()->setText(item->fullPath());
        statusBar()->showMessage(tr("Percorso copiato negli appunti: %1").arg(item->fullPath()), 3000);
    }
}

void MainWindow::copySelectedFileName()
{
    QTreeWidgetItem *current = ui->resultsTreeWidget->currentItem();
    SearchResultItem *item = dynamic_cast<SearchResultItem*>(current);
    if (item) {
        QApplication::clipboard()->setText(item->fileName());
        statusBar()->showMessage(tr("Nome file copiato negli appunti: %1").arg(item->fileName()), 3000);
    }
}

void MainWindow::onAboutClicked()
{
    QString title = tr("Informazioni su FDg");
    QString text = QStringLiteral(
        "<h3>FDg v1.2.0</h3>"
        "<p><b>%1</b></p>"
        "<hr/>"
        "<p>%2</p>"
        "<ul>"
        "<li><b>Frontend:</b> Qt 6 (C++20)</li>"
        "<li><b>Motore di ricerca:</b> fd (ultra-fast find) con supporto Wildcard MS-DOS (*, ?)</li>"
        "<li><b>Ideazione:</b> PsyTech76</li>"
        "<li><b>Sviluppo:</b> Intelligenza Artificiale</li>"
        "<li><b>Licenza:</b> Gratuita / Open Source</li>"
        "</ul>"
        "<p><i>%3</i></p>"
    ).arg(
        tr("Applicazione realizzata con IA su un'idea di PsyTech76 - Gratuita"),
        tr("Un'interfaccia grafica moderna, veloce ed intuitiva per il potente strumento di ricerca su linea di comando fd."),
        tr("Realizzato per la comunità GNU/Linux.")
    );

    QMessageBox::about(this, title, text);
}

void MainWindow::onAboutQtClicked()
{
    QMessageBox::aboutQt(this, tr("Informazioni su Qt"));
}

/**
 * @brief Imposta la directory personale dell'utente (QDir::homePath()) come percorso di ricerca.
 * Invocato dal pulsante 'Home' posizionato accanto all'etichetta del percorso.
 */
void MainWindow::onHomeFolderClicked()
{
    ui->folderLineEdit->setText(QDir::homePath());
}

/**
 * @brief Apre la finestra della guida utente integrata.
 * Inizializza il browser minimale GuideDialog con la lingua attiva (m_currentLocale),
 * consentendo la consultazione offline e il cambio di lingua rapido su ogni pagina.
 */
void MainWindow::onOpenGuideClicked()
{
    GuideDialog dialog(m_currentLocale, this);
    dialog.exec();
}

/**
 * @brief Aggiorna la visibilità del contenitore dei parametri aggiuntivi di ricerca
 * e cambia l'indicatore grafico (triangolino ▶ se compresso, ▼ se espanso).
 * Salva inoltre la preferenza in QSettings per conservare lo stato tra i riavvii.
 */
void MainWindow::updateAdvancedOptionsVisibility()
{
    ui->advancedOptionsWidget->setVisible(m_optionsExpanded);
    QString arrow = m_optionsExpanded ? QString::fromUtf8("▼ ") : QString::fromUtf8("▶ ");
    ui->toggleOptionsButton->setText(arrow + tr("Parametri aggiuntivi di ricerca"));
    ui->toggleOptionsButton->setToolTip(tr("Mostra o nascondi le opzioni avanzate di ricerca e localizzazione"));

    QSettings settings(QStringLiteral("PsyTech76"), QStringLiteral("FDg"));
    settings.setValue(QStringLiteral("ui/optionsExpanded"), m_optionsExpanded);
}

/**
 * @brief Inverte lo stato di espansione/compressione dei parametri aggiuntivi di ricerca.
 */
void MainWindow::onToggleAdvancedOptions()
{
    m_optionsExpanded = !m_optionsExpanded;
    updateAdvancedOptionsVisibility();
}

/**
 * @brief Gestisce l'azione 'Nuova Ricerca' (Menu File o Ctrl+N).
 * Interrompe eventuali ricerche in corso, azzera la query e il pulsante,
 * e cancella completamente tutti i risultati della ricerca precedente.
 */
void MainWindow::onNewSearchTriggered()
{
    // Se è in corso una ricerca, la arrestiamo immediatamente tramite SIGKILL
    if (m_runner->isSearching()) {
        m_runner->stopSearch();
    }

    // Cancella il campo cerca e disabilita il pulsante Cerca
    ui->searchLineEdit->clear();
    ui->searchButton->setEnabled(false);
    ui->searchButton->setText(tr("Cerca"));
    ui->searchButton->setStyleSheet(QString());

    // Requisito: cancellare tutti i risultati della ricerca precedente
    ui->resultsTreeWidget->setSortingEnabled(false);
    ui->resultsTreeWidget->clear();
    m_itemsCounter = 0;

    // Reimposta la barra di avanzamento e i messaggi informativi
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    ui->statusLabel->setText(tr("Pronto per la ricerca"));
    statusBar()->showMessage(tr("Pronto per una nuova ricerca"), 3000);

    // Ripristina il focus sul campo di testo
    ui->searchLineEdit->setFocus();
}

/**
 * @brief Sincronizza lo stato della voce di menu 'Integra nel sistema...'
 * verificando all'avvio e dopo ogni operazione l'esistenza del file .desktop.
 */
void MainWindow::updateDesktopIntegrationAction()
{
    bool integrated = DesktopIntegrator::isIntegrated();
    if (integrated) {
        ui->actionToggleDesktopIntegration->setText(tr("Rimuovi integrazione dal sistema..."));
        ui->actionToggleDesktopIntegration->setStatusTip(tr("Rimuove il lanciatore .desktop e l'icona dal menu e dalla barra delle applicazioni"));
    } else {
        ui->actionToggleDesktopIntegration->setText(tr("Integra nel sistema (Menu e Barra)..."));
        ui->actionToggleDesktopIntegration->setStatusTip(tr("Crea il lanciatore .desktop e l'icona per consentire l'avvio dal menu e il blocco sulla barra delle applicazioni"));
    }
}

/**
 * @brief Gestisce l'installazione o la rimozione automatica del lanciatore .desktop e dell'icona
 * per consentire il blocco permanente sulla barra delle applicazioni e nel menu di sistema.
 */
void MainWindow::onToggleDesktopIntegration()
{
    bool integrated = DesktopIntegrator::isIntegrated();
    if (integrated) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Rimuovi integrazione"),
            tr("Vuoi davvero rimuovere FDg dal menu delle applicazioni e dalla barra di sistema?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );
        if (reply == QMessageBox::Yes) {
            QString error;
            if (DesktopIntegrator::removeIntegration(error)) {
                QMessageBox::information(
                    this,
                    tr("Integrazione Desktop"),
                    tr("Integrazione rimossa con successo.\n\nIl lanciatore desktop di FDg è stato eliminato dal sistema.")
                );
            } else {
                QMessageBox::warning(
                    this,
                    tr("Errore"),
                    tr("Impossibile rimuovere l'integrazione: %1").arg(error)
                );
            }
            updateDesktopIntegrationAction();
        }
    } else {
        QString error;
        if (DesktopIntegrator::installIntegration(error)) {
            QMessageBox::information(
                this,
                tr("Integrazione Desktop"),
                tr("Integrazione completata con successo!\n\n"
                   "FDg è stato aggiunto al menu delle applicazioni.\n"
                   "Ora puoi trovarlo nel menu di avvio e bloccarlo stabilmente alla barra delle applicazioni.")
            );
        } else {
            QMessageBox::warning(
                this,
                tr("Errore"),
                tr("Impossibile completare l'integrazione: %1").arg(error)
            );
        }
        updateDesktopIntegrationAction();
    }
}

/**
 * @brief Verifica all'avvio la presenza dell'eseguibile 'fd' (o 'fdfind') nel sistema.
 * Se mancante, visualizza un dialogo che propone all'utente l'installazione automatica.
 */
void MainWindow::checkFdDependency()
{
    QString fdPath = FdRunner::getFdExecutableName();
    if (!fdPath.isEmpty()) {
        return; // Motore già presente e operativo, nessun avviso necessario all'avvio
    }

    onInstallFdClicked();
}

/**
 * @brief Verifica la presenza del motore 'fd' nel sistema.
 * Se il motore è già presente, mostra un messaggio informativo con percorso e versione senza avviare alcuna installazione.
 * Se il motore è assente, propone all'utente di procedere al download e all'installazione guidata.
 */
void MainWindow::onInstallFdClicked()
{
    QString fdPath = FdRunner::getFdExecutableName();
    if (!fdPath.isEmpty()) {
        // Verifica preliminare: il motore è già presente nel sistema operativo
        QString versionInfo;
        QProcess proc;
        proc.setProcessEnvironment(FileManagerHelper::cleanEnvironment());
        proc.start(fdPath, {QStringLiteral("--version")});
        if (proc.waitForFinished(1000)) {
            versionInfo = QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
        }
        if (versionInfo.isEmpty()) {
            versionInfo = fdPath;
        }

        QMessageBox::information(
            this,
            tr("Verifica Motore 'fd'"),
            tr("Il motore di ricerca 'fd' è già installato e configurato correttamente nel sistema operativo!\n\n"
               "• Percorso eseguibile: %1\n"
               "• Versione rilevata: %2\n\n"
               "Tutte le funzionalità di ricerca di FDg sono pienamente operative e non è necessaria alcuna installazione.").arg(fdPath, versionInfo)
        );

        statusBar()->showMessage(tr("Motore 'fd' verificato ed operativo: %1").arg(versionInfo), 5000);
        return;
    }

    // Motore assente: chiede all'utente se desidera avviare l'installazione automatica
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Motore di ricerca 'fd' mancante"),
        tr("Il motore di ricerca 'fd' (o 'fdfind') non è stato trovato nel sistema operativo.\n\n"
           "È un componente indispensabile per consentire a FDg di cercare file e cartelle.\n\n"
           "Desideri procedere al download e all'installazione automatica adesso?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes
    );

    if (reply == QMessageBox::Yes) {
        DependencyInstallerDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted || dialog.wasSuccessful()) {
            QString newFdPath = FdRunner::getFdExecutableName();
            if (!newFdPath.isEmpty()) {
                statusBar()->showMessage(tr("Motore di ricerca 'fd' configurato ed operativo (%1)").arg(newFdPath), 5000);
                updateUiText();
            }
        }
    } else {
        statusBar()->showMessage(tr("Attenzione: 'fd' non è installato. Le ricerche saranno inattive."), 6000);
        ui->statusLabel->setText(tr("Motore 'fd' non trovato. Installalo per iniziare."));
    }
}


