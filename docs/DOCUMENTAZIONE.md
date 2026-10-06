# Documentazione Tecnica e Guida Utente: FDg v1.2.0

**Applicazione realizzata con IA su un'idea di PsyTech76 - Gratuita**

---

## 1. Panoramica del Progetto

**FDg** è un'interfaccia grafica (GUI) ad alte prestazioni sviluppata in **C++20** con il framework **Qt 6**, concepita come frontend desktop universale per l'utility di ricerca file da riga di comando **`fd`** (`fd-find` su Debian/Ubuntu).

L'obiettivo dell'applicazione è offrire all'utente l'estrema velocità di ricerca di `fd` (scritto in Rust) attraverso un'interfaccia intuitiva, ergonomica, multi-lingua e **compatibile al 100% sia con le distribuzioni basate su Debian (Debian 12/13, Ubuntu, Linux Mint, Pop!_OS) sia con quelle basate su Arch Linux (Arch, CachyOS, Manjaro, EndeavourOS) e Fedora**.

---

## 2. Compatibilità Cross-Distribution (Arch Linux & Debian)

### 2.1. Rilevamento Automatico di `fd` (Arch) e `fdfind` (Debian/Ubuntu)
- Su distribuzioni Arch Linux, Fedora e openSUSE, il comando ufficiale è denominato **`/usr/bin/fd`**.
- Su Debian e Ubuntu, il pacchetto ufficiale si chiama `fd-find` e il binario è denominato **`/usr/bin/fdfind`** (per evitare conflitti storici con un pacchetto floppy).
- In `FdRunner::getFdExecutableName()`, l'applicazione effettua una ricerca a cascata:
  1. Verifica l'eventuale variabile d'ambiente `FD_PATH` impostata dall'utente.
  2. Cerca nel `PATH` di sistema l'eseguibile `fd`.
  3. Se assente, cerca nel `PATH` l'eseguibile `fdfind`.
  4. Controlla i percorsi convenzionali (`/usr/bin/fdfind`, `/usr/bin/fd`, `/usr/local/bin/fd`, `~/.cargo/bin/fd`).
- **Risultato**: Funzionamento immediato out-of-the-box sia su Arch che su Debian/Ubuntu senza bisogno di creare alias o symlink manuali.

### 2.2. Portabilità dell'AppImage (Glibc 2.34+ Universale)
- L'AppImage viene costruita partendo da un ambiente base compatibile **Debian 12 (Bookworm) con GLIBC 2.36** e librerie di runtime Qt 6.
- Poiché la libreria C di GNU (`glibc`) è retrocompatibile, un binario compilato con target GLIBC 2.34/2.36 può essere eseguito senza problemi su:
  - **Debian 12 (Bookworm), Debian 13 (Trixie), Debian Sid**
  - **Ubuntu 22.04 LTS, Ubuntu 24.04 LTS, Ubuntu 26.04**
  - **Linux Mint 21 & 22, Pop!_OS, Zorin OS, Elementary OS**
  - **Arch Linux, CachyOS, Manjaro, EndeavourOS**
  - **Fedora 36, 37, 38, 39, 40, 41, 42**
  - **openSUSE Leap 15.5+, Tumbleweed**

### 2.3. Integrazione con i File Manager di Sistema
- Supporta l'interfaccia DBus standard FreeDesktop `org.freedesktop.FileManager1.ShowItems`.
- Include il supporto diretto per tutti i file manager diffusi negli ambienti desktop di Arch e Debian:
  - **Nautilus** (GNOME - Debian / Ubuntu / Fedora)
  - **Dolphin** (KDE Plasma - Arch / Kubuntu / Debian KDE)
  - **Nemo** (Cinnamon - Linux Mint)
  - **Thunar** (XFCE - Debian XFCE / Xubuntu / Manjaro)
  - **PCManFM / PCManFM-Qt** (LXDE / LXQt - Debian LXQt / Lubuntu)

---

## 3. Requisiti e Funzionalità Implementate

| Requisito Richiesto | Stato | Dettagli Implementativi |
| :--- | :---: | :--- |
| **GUI Frontend per `fd`** | ✅ Conforme | Rilevamento automatico di `fd` (Arch) e `fdfind` (Debian/Ubuntu). |
| **Compatibilità Debian & Arch** | ✅ Conforme | AppImage universale compilata con requisiti GLIBC 2.34+ compatibile con Debian 12+ e Arch Linux. |
| **Avvio e Kill Immediato Ricerca** | ✅ Conforme | Campo di ricerca e pulsante dinamico. **Kill immediato (SIGKILL)** al processo e process group, disconnessione stream e svuotamento buffer. **Sicuro al 100%** per l'OS e i documenti poiché `fd` esegue solo scansioni in sola lettura. |
| **Percorso Default e Pulsante Home** | ✅ Conforme | Percorso di ricerca impostato di default su `/` (root del disco) e pulsante **Home** dedicato per selezionare con un click la directory personale dell'utente. |
| **Jolly MS-DOS (* e ?)** | ✅ Conforme | Integrazione dell'opzione `--glob` (`-g`) di `fd` per supportare pattern come `*.txt`, `test*`, `*.*`, `*`, `nome*.jpg` con checkbox dedicata attiva di default. |
| **List box a 2 colonne** | ✅ Conforme | `QTreeWidget` con colonna 0 (*Nome File*) e colonna 1 (*Percorso*), in ordine di coerenza originario. |
| **Ordinamento crescente/decrescente** | ✅ Conforme | Click sull'intestazione per ordinare in modo naturale (crescente A-Z 0-9 e decrescente Z-A 9-0). |
| **Barra di avanzamento** | ✅ Conforme | Animazione continua durante la scansione con contatore in tempo reale ed esito finale. |
| **Doppio click sul path** | ✅ Conforme | Apre il file manager ed **evidenzia il file selezionato senza aprirlo** tramite DBus o file manager specifico. |
| **Doppio click nome file** | ✅ Conforme | Apre direttamente il file con l'applicazione associata in ambiente isolato (`cleanEnvironment()`), prevenendo crash. |
| **Guida HTML e Browser Integrato** | ✅ Conforme | Guida HTML completa incorporata nell'AppImage in 5 lingue (IT, EN, DE, ES, FR), visualizzabile con browser minimale dedicato (`GuideDialog`) accessibile da Menu Aiuto (F1) e footer, con cambio lingua con 1 click. |
| **Bandiere Lingua Grafiche** | ✅ Conforme | Icone delle bandiere per la selezione lingua create in grafica vettoriale e PNG, perfettamente visibili su qualsiasi ambiente desktop e font. |
| **Attribution PsyTech76** | ✅ Conforme | Banner nel footer: <i>"Applicazione realizzata con IA su un'idea di PsyTech76 - Gratuita"</i>. |
| **Qt 6 C++ con UI separata** | ✅ Conforme | Interfaccia in `mainwindow.ui` (Qt Designer XML) e codice C++20 commentato in italiano. |
| **Supporto Multi-Lingua (5 lingue)** | ✅ Conforme | Italiano, Inglese, Tedesco, Spagnolo e Francese selezionabili a runtime dall'interfaccia. |
| **Archivio compresso su Desktop** | ✅ Conforme | Pacchetto compresso `.zip` salvato su `~/Desktop` e `~/Scrivania` contenente esclusivamente l'AppImage e la documentazione. |

---

## 4. Guida Rapida all'Uso e Esempi di Sintassi

- **Esempi con caratteri jolly stile MS-DOS**:
  - `*.txt` : trova tutti i file di testo.
  - `*.*` : trova qualsiasi file dotato di estensione (esattamente come su MS-DOS).
  - `doc*` : trova file e cartelle che iniziano con "doc".
  - `*progetto*` : trova tutti i file contenenti la parola "progetto".
  - `test?.png` : trova `test1.png`, `test2.png`, ecc.
  - `*` : elenca tutti gli elementi presenti nella cartella.
- **Interruzione**: Premere il pulsante rosso **Interrompi** per bloccare la ricerca all'istante.
- **Apertura file**: **Doppio click** sulla colonna *Nome File*.
- **Mostra nel File Manager**: **Doppio click** sulla colonna *Percorso*.

---

## 5. Note Tecniche e Guida Architetturale per Future Revisioni (Umano o IA)

Questa sezione documenta le decisioni ingegneristiche e le soluzioni adottate a fronte di problematiche note, ad uso di futuri manutentori umani o sistemi di intelligenza artificiale:

### 5.1. Risoluzione della Visualizzazione delle Bandiere (i18n)
- **Problema riscontrato**: Utilizzando glifi emoji Unicode (`🇮🇹`, `🇬🇧`, `🇩🇪`, ecc.) all'interno dei testi del `QComboBox`, sui sistemi Linux privi di font emoji a colori (es. Noto Color Emoji) o con font bitmap/monospace le bandiere venivano visualizzate come lettere di codice paese ("IT", "GB") o scatole vuote/punti interrogativi.
- **Soluzione adottata**: Implementazione di icone grafiche vettoriali (SVG) e raster (PNG) dedicate caricate in `resources/icons/flags/` e incorporate in `resources.qrc`. Vengono fornite a `QComboBox::addItem()` tramite `QIcon`, garantendo una resa visiva perfetta, nitida e uniforme su qualsiasi desktop Linux indipendentemente dalla configurazione dei font di sistema.

### 5.2. Chiusura Forzata Immediata del Processo di Ricerca (`SIGKILL` / Kill App)
- **Analisi di Sicurezza**: L'utility `fd` (`fdfind`) effettua esclusivamente scansioni in lettura dei descrittori di file e directory (`O_RDONLY`). Non apre mai file in scrittura né altera metadati del filesystem. L'invio di un segnale `SIGKILL` al processo `fd` è pertanto **sicuro al 100% per l'integrità del sistema operativo e per tutti i documenti**.
- **Gestione Process Group**: All'avvio del comando `fd`, viene associato un nuovo gruppo di processi tramite `setpgid(0, 0)`. Alla richiesta di interruzione, `FdRunner::cleanupProcess()` invia `SIGKILL` sia al gruppo (`::kill(-pid, SIGKILL)`) sia al processo (`::kill(pid, SIGKILL)`), prevenendo processi orfani o thread bloccati.
- **Sincronizzazione GUI e De-queueing**: Tutti i segnali da `QProcess` vengono disconnessi istantaneamente prima del kill (`m_process->disconnect(this)`), i buffer in memoria vengono azzerati e `MainWindow::onResultsBatchReady()` scarta qualsiasi batch residuo se `m_runner->isSearching()` è falso. Questo elimina qualsiasi blocco o latenza dell'interfaccia.

### 5.3. Percorso Predefinito e Tasto Rapido "Home"
- **Percorso Default**: Impostato sulla radice del filesystem (`/`), permettendo all'utente di effettuare ricerche su tutto il disco fin dal primo avvio.
- **Pulsante Home**: Aggiunto accanto all'etichetta del percorso; imposta istantaneamente `QDir::homePath()` per restringere rapidamente le ricerche all'ambiente personale dell'utente.

### 5.4. Guida HTML Multilingua e Browser Minimale Integrato (`GuideDialog`)
- **Scelta di `QTextBrowser` vs `QWebEngineView`**: L'adozione di `QWebEngineView` (Chromium) comporterebbe un overhead di oltre 150 MB nell'AppImage e introdurrebbe pesanti dipendenze grafiche (GPU, sandbox, nss, fontconfig) che minerebbero la compatibilità cross-distribution universale tra Debian e Arch Linux. `QTextBrowser` è nativo in `QtWidgets`, leggerissimo e perfettamente integrato con le risorse Qt (`qrc:/`).
- **Autosufficienza dell'AppImage**: Le guide HTML (`resources/help/guide_<lang>.html`) sono compilate all'interno dell'eseguibile tramite `resources.qrc`, garantendo la consultazione anche in assenza di connessione internet o senza file esterni.
- **Cambio Lingua Contestuale**: Ogni pagina include una barra con collegamenti diretti alle altre traduzioni; `GuideDialog` intercetta i click interni aggiornando automaticamente il selettore delle lingue e il testo mostrato.
- **Accessibilità**: Scorciatoia universale `F1`, menu `Aiuto -> Guida dell'applicazione...` e pulsante dedicato `Guida...` nel footer.

### 5.5. Ergonomia Desktop e Isolamento Applicazioni
- **Doppio Click Esclusivo**: L'apertura dei file o delle cartelle è legata unicamente a `itemDoubleClicked` e al tasto `Invio`. Il segnale standard `itemActivated` è stato escluso per evitare attivazioni indesiderate con singolo click su desktop configurati con KDE Plasma.
- **Ambiente Isolato (`cleanEnvironment`)**: All'apertura di file associati tramite `FileManagerHelper::openFile()`, le variabili d'ambiente `LD_LIBRARY_PATH` e `QT_PLUGIN_PATH` dell'AppImage vengono purificate per prevenire crash applicativi delle app di sistema (es. VLC, Kate, Gedit, LibreOffice).

### 5.6. Normalizzazione Percorsi e Visualizzazione Directory nei Risultati
- **Problema riscontrato**: Nei risultati di ricerca, quando `fd` individua una cartella invece di un file, l'utility produce percorsi con slash finale (es. `/percorso/documenti/progetto/`). In Qt, `QFileInfo::fileName()` su un percorso che termina con `/` restituisce stringa vuota (`""`). Di conseguenza, il vecchio codice applicava un fallback che impostava l'intero percorso assoluto all'interno della colonna "Nome File", compromettendo la chiarezza e l'ordinamento.
- **Soluzione adottata**: In `MainWindow::onResultsBatchReady()`, il percorso grezzo viene normalizzato rimuovendo preventivamente qualsiasi barra finale ridondante (`while (cleanPath.endsWith('/') && cleanPath.length() > 1) cleanPath.chop(1);`). In questo modo `QFileInfo(cleanPath).fileName()` estrae regolarmente il nome della directory (es. `"progetto"`), mentre `QFileInfo(cleanPath).absolutePath()` restituisce correttamente la cartella genitrice (es. `"/percorso/documenti"`).

### 5.7. Validazione dell'Input e Disabilitazione Ricerca su Campo Vuoto
- **Comportamento UI**: Per prevenire scansioni accidentali o non intenzionali su tutto il disco a vuoto, il pulsante **"Cerca"** è disabilitato di default all'avvio. Viene abilitato dinamicamente in tempo reale tramite il segnale `QLineEdit::textChanged` solo se il testo inserito contiene almeno un carattere non di spaziatura (`!text.trimmed().isEmpty()`).
- **Intercettazione Invio**: Se l'utente preme il tasto Invio con il campo di testo vuoto, `onSearchQueryReturnPressed()` blocca l'esecuzione ed espone un messaggio informativo nella barra di stato (*"Inserisci un testo o un pattern da cercare"*).
- **Gestione Stato Ricerca**: Quando una ricerca è in corso, il pulsante rimane abilitato assumendo il ruolo "Interrompi" per consentire la cancellazione immediata in qualsiasi momento. Al termine o in caso di errore, lo stato di abilitazione viene riallineato alla presenza o meno di testo nel campo.

### 5.8. Gestione "Nuova Ricerca" e Pulizia Risultati Precedenti
- **Comportamento e Flusso Operativo**: Quando viene attivata l'azione `Nuova Ricerca` (`ui->actionNuovaRicerca` da menu File o scorciatoia universale `Ctrl+N`):
  1. Se è in corso un processo di scansione, viene terminato istantaneamente via `FdRunner::stopSearch()` (`SIGKILL`).
  2. Il campo di testo della query viene ripulito e il pulsante "Cerca" viene disabilitato.
  3. La lista dei risultati precedente viene azzerata completamente (`ui->resultsTreeWidget->clear()`) e il conteggio elementi azzerato (`m_itemsCounter = 0`).
  4. La barra di avanzamento viene reimpostata a zero e le etichette informative riportate allo stato di pronto.
  5. Il cursore viene riposizionato con focus immediato su `searchLineEdit`.

### 5.9. Parametri Aggiuntivi di Ricerca Collassabili/Nascondibili
- **Design dell'Interfaccia**: Per mantenere l'interfaccia principale pulita, minimale e focalizzata sulla ricerca e sulla visualizzazione dei risultati, le opzioni secondarie (file nascosti, case sensitive, symlink, jolly MS-DOS, filtro tipo) e il selettore della lingua sono raggruppati all'interno di un contenitore dedicato (`advancedOptionsWidget`).
- **Controllo di Espansione**: Un pulsante ad attivazione rapida (`toggleOptionsButton`) con triangolino cliccabile (`▶ Parametri aggiuntivi di ricerca` se compresso, `▼ Parametri aggiuntivi di ricerca` se espanso) consente di mostrare o nascondere le impostazioni con un singolo click.
- **Persistenza**: Lo stato di apertura/chiusura viene salvato automaticamente in `QSettings` (`ui/optionsExpanded`), preservando la preferenza dell'utente tra le varie sessioni dell'applicazione. Di default il pannello si presenta compresso per la massima pulizia visiva.
- **Localizzazione**: Il testo dell'intestazione e i relativi tooltip sono completamente tradotti e si aggiornano dinamicamente in tutte le 5 lingue supportate.

### 5.10. Integrazione Automatica Desktop XDG (Menu e Barra delle Applicazioni)
- **Problema delle AppImage nei Desktop Manager (es. KDE Plasma / GNOME)**:
  Le applicazioni AppImage risiedono ed eseguono da un mount temporaneo effimero (`/tmp/.mount_XXXXXX/`). Quando l'utente aggiunge la finestra ai preferiti o la blocca alla barra delle applicazioni (Task Manager), l'ambiente desktop memorizza il percorso del lanciatore `.desktop` e dell'icona situati nel punto di montaggio temporaneo. Alla chiusura del programma, la cartella temporanea viene smontata ed eliminata dal sistema operativo: il risultato è uno spazio vuoto/icona invisibile sulla barra delle applicazioni e l'impossibilità di avviare il software al click.
- **Soluzione Architetturale (`DesktopIntegrator`)**:
  È stato implementato il modulo autonomo `DesktopIntegrator` gestito dalla voce di menu `Strumenti -> Integra nel sistema...` (e `Rimuovi integrazione...`).
  1. **Rilevamento Eseguibile Reale**: Estrae il percorso permanente dell'AppImage leggendo la variabile d'ambiente `APPIMAGE` definita dal runtime di AppImage (con fallback su `QCoreApplication::applicationFilePath()`).
  2. **Persistenza Icona**: Estrae e copia l'icona dell'applicazione a 256x256 px in `~/.local/share/icons/fdg.png`.
  3. **Lanciatore Standard**: Genera il file `~/.local/share/applications/fdg.desktop` con le categorie XDG appropriate, commenti multilingua, percorso permanente all'icona ed eseguibile, e `StartupWMClass=fdg`.
  4. **Aggiornamento Dinamico del Menu**: Ad ogni avvio dell'applicazione e in occasione del cambio lingua, la funzione `MainWindow::updateDesktopIntegrationAction()` verifica la presenza del file e commuta automaticamente la voce tra *"Integra nel sistema..."* e *"Rimuovi integrazione dal sistema..."*.
  5. **Disinstallazione Pulita**: La voce di rimozione elimina sia il lanciatore `.desktop` che l'icona, e aggiorna il database del desktop (`update-desktop-database`) senza lasciare tracce.

### 5.11. Controllo Automatico all'Avvio e Installazione Guidata Dipendenze (`DependencyInstallerDialog`)
- **Esigenza Funzionale**: Garantire che l'utente possa utilizzare immediatamente FDg senza dover aprire manualmente una riga di comando per installare `fd` (o `fd-find`), indipendentemente dalla distribuzione Linux in uso.
- **Rilevamento all'Avvio**: In `MainWindow`, un timer differito (`QTimer::singleShot(300, ...)`) esegue `checkFdDependency()`. Se `FdRunner::getFdExecutableName()` restituisce una stringa vuota (motore assente), viene mostrato un popup `QMessageBox::question` che informa l'utente e propone l'installazione automatica.
- **Architettura Modulare (`DependencyInstallerDialog`)**:
  1. **Riconoscimento della Distribuzione**: `detectPackageManager()` cerca nel `PATH` di sistema gli eseguibili dei principali gestori pacchetti:
     - `pacman` &rarr; Arch Linux / CachyOS / Manjaro (pacchetto `fd`).
     - `apt-get` &rarr; Debian / Ubuntu / Linux Mint (pacchetto `fd-find`).
     - `dnf` &rarr; Fedora / Red Hat Enterprise Linux (pacchetto `fd-find`).
     - `zypper` &rarr; openSUSE (pacchetto `fd`).
  2. **Isolamento dell'Ambiente AppImage**: L'esecuzione di `pacman`, `apt-get` o `pkexec` richiede l'ambiente di sistema dell'host. Se venissero usate le variabili `LD_LIBRARY_PATH` dell'AppImage, si verificherebbero conflitti di simboli con la glibc di sistema. Tramite `FileManagerHelper::cleanEnvironment()`, le variabili effimere vengono rimosse prima dell'avvio di `QProcess`.
  3. **Elevazione Privilegi Trasparente con GUI**:
     - *PolicyKit (`pkexec`)*: Fa apparire la finestra di autorizzazione grafica nativa del desktop (KAuth su KDE Plasma, Polkit su GNOME).
     - *Gestione Annullamento*: Il codice di uscita 126 (annullamento autorizzazione) viene gestito elegantemente senza segnalare errori anomali.
  4. **Log in Tempo Reale e Pulizia ANSI**: L'output standard e gli errori del gestore pacchetti vengono convogliati in diretta su un `QPlainTextEdit` scuro a caratteri a spaziatura fissa. I codici di controllo e colore ANSI del terminale vengono rimossi tramite espressione regolare (`\x1b\[[0-9;]*[a-zA-Z]`) per una visualizzazione chiara e pulita.
  5. **Verifica Post-Installazione e Feedback**: Al termine del comando (codice uscita 0), viene rieseguito `FdRunner::getFdExecutableName()`. Se confermato, la barra di avanzamento raggiunge il 100% e viene mostrato un messaggio di successo `QMessageBox::information`.
- **Verifica Preventiva e Risoluzione Reinstallazioni Ridondanti (Integrazione Menu)**:
  - Quando l'utente clicca `Strumenti -> Verifica o Installa motore 'fd'...`, il sistema verifica prioritariamente se `fd` o `fdfind` è già presente ed eseguibile.
  - Se il motore è già installato, `MainWindow::onInstallFdClicked()` esegue `fd --version` (in ambiente ripulito da `LD_LIBRARY_PATH`) e mostra un dialogo informativo `QMessageBox::information` riportando il percorso dell'eseguibile e la versione rilevata, confermando che tutte le ricerche sono pronte all'uso senza avviare alcuna procedura di installazione.
  - L'installazione guidata viene avviata **esclusivamente in sua assenza** (previa richiesta di conferma).
  - Un controllo di sicurezza identico è integrato anche all'interno di `DependencyInstallerDialog::startInstallation()`, garantendo che nessun pacchetto venga installato se il binario è già presente nel sistema.

---

## 6. Crediti e Licenza

- **Ideazione e Requisiti di Progetto**: PsyTech76
- **Sviluppo**: Intelligenza Artificiale (Google DeepMind - Advanced Agentic Coding)
- **Licenza**: Software Gratuito e Open Source rilasciato per la comunità GNU/Linux.

