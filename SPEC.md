# Specifiche Tecniche e Funzionali del Progetto (SPEC.md)

**Progetto**: FDg (GUI Desktop per il motore di ricerca `fd`)  
**Versione**: 1.2.0  
**Linguaggio & Standard**: C++17 / C++20  
**Framework GUI**: Qt 6 (Core, Gui, Widgets, DBus)  
**Licenza**: MIT License  
**Autore / Ideazione**: PsyTech76 (Sviluppato con il supporto dell'Intelligenza Artificiale)  
**Target OS**: Linux (Debian, Ubuntu, Arch Linux, Fedora, openSUSE e derivate)  

---

## 1. Visione Generale e Obiettivi del Progetto

### 1.1. Scopo
**FDg** è un'interfaccia grafica (GUI) ad alte prestazioni sviluppata per rendere accessibile, immediata ed ergonomica la potenza di calcolo del motore di ricerca file **`fd`** (`fd-find` su distribuzioni Debian/Ubuntu), scritto in linguaggio Rust.

L'applicazione trasforma uno strumento da riga di comando (CLI) in una moderna applicazione desktop universale, fornendo funzionalità avanzate quali ordinamento interattivo, integrazione diretta con i gestori file di sistema (con pre-selezione degli elementi), installazione assistita dei prerequisiti con PolicyKit, e integrazione dinamica con la barra delle applicazioni e il menu di sistema Linux.

### 1.2. Obiettivi Primari
- **Massima velocità di scansione**: Sfruttare appieno il motore multithread di `fd` senza colli di bottiglia nell'interfaccia utente.
- **Portabilità Universale Linux**: Funzionamento garantito out-of-the-box sia su sistemi basati su Debian/Ubuntu che su Arch Linux, Fedora e derivate, tramite formato autonomo AppImage.
- **Sicurezza e Riservatezza**: Elevazione dei privilegi sicura delegata interamente a PolicyKit (`pkexec`), nessun salvataggio o gestione interna di credenziali utente, scansioni esclusivamente in sola lettura (`O_RDONLY`).
- **Interfaccia Pulita ed Ergonomica**: Controlli di ricerca essenziali visibili subito, parametri avanzati raggruppati in un pannello comprimibile a scomparsa.
- **Multilingua Nativo**: Supporto completo per 5 lingue (Italiano, Inglese, Tedesco, Spagnolo, Francese) sia nell'interfaccia che nella guida integrata.

---

## 2. Architettura del Software

Il progetto segue un'architettura modulare orientata agli eventi (Event-Driven Architecture) basata sul meccanismo dei Segnali e Slot di Qt 6.

```
                           +---------------------------+
                           |        MainWindow         |
                           |  (UI Controller & State)  |
                           +-------------+-------------+
                                         |
     +-------------------+---------------+-------------------+-------------------+
     |                   |                                   |                   |
     v                   v                                   v                   v
+------------+  +-------------------+               +------------------+  +--------------+
|  FdRunner  |  | FileManagerHelper |               |DesktopIntegrator |  | Dependency-  |
| (QProcess) |  | (DBus & Fallback) |               |  (XDG .desktop)  |  |   Installer  |
+-----+------+  +-------------------+               +------------------+  +-------+------+
      |                                                                           |
      v                                                                           v
 [fd / fdfind]                                                              [pkexec apt/pacman]
```

### 2.1. Componenti Principali

| Modulo / File | Responsabilità |
| :--- | :--- |
| [`src/main.cpp`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/main.cpp) | Entry point dell'applicazione; inizializzazione di `QApplication`, configurazione scale DPI, installazione traduzioni e istanziazione della finestra principale. |
| [`src/mainwindow.h`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/mainwindow.h) / [`.cpp`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/mainwindow.cpp) / [`.ui`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/mainwindow.ui) | Controller principale dell'interfaccia utente; gestione eventi di input, binding dei controlli di ricerca, pannello comprimibile, visualizzazione e ordinamento risultati (`QTreeWidget`), barra di stato e menu. |
| [`src/fdrunner.h`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/fdrunner.h) / [`.cpp`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/fdrunner.cpp) | Gestore asincrono del sottoprocesso di ricerca; composizione degli argomenti CLI, avvio con gruppo di processi isolato (`setpgid`), buffering a blocchi dei risultati e terminazione forzata immediata (`SIGKILL`). |
| [`src/filemanagerhelper.h`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/filemanagerhelper.h) / [`.cpp`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/filemanagerhelper.cpp) | Astrazione per l'apertura delle directory; implementa l'interfaccia DBus standard FreeDesktop (`org.freedesktop.FileManager1.ShowItems`) per evidenziare i file ed esegue fallback mirati sui file manager più comuni (Nautilus, Dolphin, Nemo, Thunar, PCManFM). |
| [`src/desktopintegrator.h`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/desktopintegrator.h) / [`.cpp`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/desktopintegrator.cpp) | Gestore del ciclo di vita del file lanciatore XDG (`.desktop`); installa o rimuove il collegamento in `~/.local/share/applications/` per consentire l'ancoraggio alla barra delle applicazioni e al menu di sistema. |
| [`src/dependencyinstaller.h`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/dependencyinstaller.h) / [`.cpp`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/dependencyinstaller.cpp) | Gestore del rilevamento e dell'installazione del binario `fd`/`fdfind`; esegue verifiche preliminari sul PATH e invoca `pkexec` su APT o Pacman con dialog di avanzamento in tempo reale. |
| [`src/themehelper.h`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/themehelper.h) / [`.cpp`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/themehelper.cpp) | Gestore centralizzato del tema e dello stile grafico; impone lo stile Fusion chiaro predefinito per perfetta coerenza con la documentazione e consente la selezione di temi Scuro e Sistema. |
| [`src/guidedialog.h`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/guidedialog.h) / [`.cpp`](file:///home/tonibu/.gemini/antigravity/scratch/fd-frontend/src/guidedialog.cpp) | Visualizzatore integrato della guida utente HTML; renderizza la documentazione locale caricata dalle risorse Qt in 5 lingue differenti con selezione a runtime. |

---

## 3. Specifiche Funzionali

### 3.1. Rilevamento Motore di Ricerca ed Esecuzione
- **Risoluzione Binario**: L'applicazione supporta sia Arch Linux/Fedora (`fd`) sia Debian/Ubuntu (`fdfind`).
  - Ricerca a cascata:
    1. Variabile d'ambiente `FD_PATH` (se impostata).
    2. Eseguibile `fd` nel `PATH` di sistema (`QStandardPaths::findExecutable("fd")`).
    3. Eseguibile `fdfind` nel `PATH` di sistema (`QStandardPaths::findExecutable("fdfind")`).
    4. Percorsi assoluti standard (`/usr/bin/fd`, `/usr/bin/fdfind`, `/usr/local/bin/fd`, `~/.cargo/bin/fd`).
- **Verifica all'Avvio e Menu**:
  - All'avvio dell'applicazione, se il motore `fd` non è presente nel sistema, compare una finestra di dialogo che chiede all'utente se desidera installarlo.
  - La voce di menu *"Verifica o installa motore fd"* verifica preventivamente se `fd` è già presente. Se presente, notifica l'utente senza avviare alcuna procedura; se assente, avvia la procedura di installazione automatizzata.

### 3.2. Installazione Automatizzata dei Prerequisiti
- **Rilevamento Gestore Pacchetti**:
  - `pacman` (Arch Linux, Manjaro, EndeavourOS, CachyOS) -> pacchetto `fd`.
  - `apt` / `apt-get` (Debian, Ubuntu, Linux Mint, Pop!_OS) -> pacchetto `fd-find`.
- **Elevazione Privilegi**:
  - L'installazione viene eseguita tramite `pkexec` (PolicyKit di sistema), garantendo una finestra grafica di autenticazione sicura gestita dal desktop manager.
  - Nessuna password o dato sensibile viene mai gestito, intercettato o salvato in memoria dall'applicazione.
- **Monitoraggio in Tempo Reale**:
  - L'output del gestore pacchetti viene letto in streaming riga per riga da un `QProcess` asincrono.
  - Una finestra di dialogo con barra di avanzamento e log a scorrimento mostra lo stato dell'operazione e segnala l'esito finale (successo/errore).

### 3.3. Configurazione della Ricerca e UI
- **Pannello di Ricerca Primario**:
  - Campo di testo *Cerca*: input per il pattern o nome file. Disabilitazione automatica della ricerca se il campo è vuoto o composto solo da spazi.
  - Percorso di ricerca: preimpostato di default su `/` (radice) per ricerche globali immediate.
  - Pulsante *Home*: imposta istantaneamente il percorso sulla cartella home dell'utente (`QDir::homePath()`).
  - Pulsante *Sfoglia...*: apre un `QFileDialog` per selezionare qualsiasi directory.
- **Pannello Parametri Aggiuntivi (Comprimibile)**:
  - Raggruppato in un contenitore espandibile/comprimibile tramite pulsante dedicato con indicatore a triangolo (▸ / ▾).
  - Nascosto di default all'avvio per mantenere l'interfaccia pulita e minimale.
  - Parametri inclusi:
    - *Caratteri jolly (* e ?)*: abilita l'opzione `--glob` (`-g`) di `fd` (attiva di default).
    - *Espressione regolare (Regex)*: disabilita il glob per interpretare pattern regex nativi.
    - *Distingui maiuscole/minuscole (Case sensitive)*: flag `-s` / `--case-sensitive`.
    - *Includi file nascosti*: flag `-H` / `--hidden`.
    - *Ignora file .gitignore*: flag `-I` / `--no-ignore`.
    - *Segui collegamenti simbolici*: flag `-L` / `--follow`.
    - *Cerca solo file*: flag `-t f`.
    - *Cerca solo cartelle*: flag `-t d`.
    - *Estensione file*: specifica il filtro `-e <ext>`.

### 3.4. Gestione dei Risultati e Interazione Utente
- **Nuova Ricerca**: Selezionando *File -> Nuova Ricerca*, i campi vengono resettati e la tabella dei risultati precedenti viene immediatamente svuotata.
- **Visualizzazione Tabellare**:
  - Widget: `QTreeWidget` a due colonne:
    - **Colonna 0**: *Nome File* (ordinabile, mostra solo il basename).
    - **Colonna 1**: *Percorso* (cartella genitore contenente il file).
  - Ordinamento: Supporto per ordinamento naturale crescente (A-Z) e decrescente (Z-A) cliccando sulle intestazioni.
- **Azioni Doppio Click**:
  - **Doppio click su Nome File (Colonna 0)**: Apre il file con l'applicazione predefinita di sistema tramite `QDesktopServices::openUrl()`, utilizzando un ambiente ripulito (`cleanEnvironment()`) per evitare interferenze con le variabili d'ambiente dell'AppImage.
  - **Doppio click su Percorso (Colonna 1)**: Apre il file manager di sistema posizionandosi sulla cartella ed **evidenziando/selezionando il file** senza aprirlo.

### 3.5. Integrazione con la Barra delle Applicazioni (Desktop Launcher)
- **Stato Dinamico**: All'avvio dell'applicazione e all'apertura del menu, il programma verifica se il file `~/.local/share/applications/fdg.desktop` esiste ed è valido.
- **Voce di Menu Contestuale**:
  - Se il lanciatore non è installato: la voce mostra *"Aggiungi alla barra delle applicazioni / menu"*.
  - Se il lanciatore è già installato: la voce mostra *"Rimuovi dalla barra delle applicazioni / menu"*.
- **Contenuto del Lanciatore XDG**:
  - Percorso `Exec`: punta all'eseguibile corrente dell'applicazione (o al percorso dell'AppImage rilevato da `APPIMAGE` env var).
  - Percorso `Icon`: icona ad alta risoluzione salvata in `~/.local/share/icons/hicolor/256x256/apps/fdg.png`.
  - Aggiornamento della cache desktop: esecuzione di `update-desktop-database` (se disponibile).

### 3.6. Guida Utente e Internazionalizzazione (i18n)
- **5 Lingue Supportate**: Italiano (IT), Inglese (EN), Tedesco (DE), Spagnolo (ES), Francese (FR).
- **Icone Bandiere**: Grafiche dedicate vettoriali (SVG/PNG) integrate nelle risorse per garantire la corretta visualizzazione grafica indipendentemente dai font emoji di sistema.
- **Visualizzatore Guida Integrato**: Dialog non modale con visualizzatore HTML locale, navigazione ipertestuale, tabelle e selettore rapido della lingua.

### 3.7. Gestione Stile Grafico e Temi (Theme Engine)
- **Risoluzione Eterogeneità Visiva Desktop**: I diversi ambienti Linux (KDE Plasma con Breeze Dark, GNOME con Adwaita, XFCE) applicano stili e palette proprietarie che causano discrepanze visive rispetto alla documentazione.
- **Stile Predefinito Fusion Chiaro**: All'avvio dell'applicazione viene imposto programmaticamente lo stile `Fusion` abbinato a una palette chiara calibrata (sfondo `#efefef`, base `#ffffff`, testo `#000000`, pulsanti con rilievo nitido), assicurando identità al 100% rispetto alle schermate del progetto.
- **Selettore Temi Integrato**:
  - Voce di menu `Strumenti -> Tema` con opzioni *Chiaro (Predefinito)*, *Scuro* e *Predefinito di Sistema*.
  - Persistenza della preferenza utente salvata in `QSettings` (`ui/themeMode`).
  - Traduzione dinamica delle voci del menu in tutte e 5 le lingue supportate.

---

## 4. Specifiche Tecniche e di Basso Livello

### 4.1. Modello di Esecuzione di `FdRunner`
- **Ciclo di Vita del Processo**:
  - Istanza di `QProcess`.
  - All'avvio, viene chiamato `setChildProcessModifier()` per invocare `setpgid(0, 0)`, creando un Process Group separato.
- **Gestione del Buffer e Batching**:
  - L'output standard di `fd` viene letto tramite `readyReadStandardOutput()`.
  - Le righe vengono accumulate in un buffer e inviate alla GUI a blocchi (batch) temporizzati o per quota (es. 200 righe per volta) tramite il segnale `resultsBatchReady(QStringList)`.
  - Questo previene il congelamento dell'interfaccia grafica (UI freezing) in caso di ricerche che restituiscono centinaia di migliaia di risultati.
- **Interruzione Immediata (`SIGKILL`)**:
  - Poiché l'utility `fd` effettua solo scansioni in lettura (`O_RDONLY`), la terminazione immediata è priva di rischi di corruzione dati.
  - Procedura di kill:
    1. Disconnessione immediata di tutti i segnali da `QProcess` (`m_process->disconnect(this)`).
    2. Invio di segnale `SIGKILL` al gruppo di processi: `::kill(-pid, SIGKILL)`.
    3. Invio di `SIGKILL` al singolo processo: `::kill(pid, SIGKILL)`.
    4. Svuotamento dei buffer in memoria e reset dello stato.

### 4.2. Specifiche di Integrazione con i File Manager
Per evidenziare i file nella cartella di appartenenza, `FileManagerHelper` applica la seguente strategia:
1. **DBus FreeDesktop (Metodo Primario)**:
   - Servizio: `org.freedesktop.FileManager1`
   - Percorso: `/org/freedesktop/FileManager1`
   - Interfaccia: `org.freedesktop.FileManager1`
   - Metodo: `ShowItems(QStringList URIs, QString StartupId)`
2. **Fallback tramite CLI del File Manager**:
   - `nautilus --select <file>`
   - `dolphin --select <file>`
   - `nemo --no-desktop <file>`
   - `thunar <parent_folder>`
   - `pcmanfm <parent_folder>`
3. **Fallback generico**:
   - Apertura della cartella contenitrice tramite `QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath))`.

---

## 5. Specifiche dei Requisiti Non Funzionali

### 5.1. Prestazioni
- **Latenza di avvio**: < 300 ms per la visualizzazione della GUI principale.
- **Throughput dei risultati**: Elaborazione e inserimento in vista di oltre 10.000 record/secondo senza perdita di reattività dei controlli dell'interfaccia.
- **Impronta di memoria**: Consumo di memoria RAM a riposo inferiore a 45 MB RSS.

### 5.2. Sicurezza e Privacy
- **Nessuna memorizzazione di credenziali**: Nessuna password viene memorizzata su disco, in cache o in variabili di memoria.
- **Isolamento dei privilegi**: Qualsiasi operazione amministrativa (installazione di `fd`) è confinata all'ambiente di `pkexec`.
- **Sanitizzazione dell'ambiente di esecuzione**: I file aperti dall'utente vengono avviati con un ambiente privo di variabili `LD_LIBRARY_PATH` o percorsi AppImage interni per evitare conflitti con librerie di terze parti.
- **Scansione non distruttiva**: L'applicazione non esegue operazioni di scrittura, modifica o cancellazione sui file individuati.

### 5.3. Compatibilità e Requisiti di Sistema
- **Kernel Linux**: 4.19 o superiore.
- **C Libreria (GLIBC)**: Versione 2.34 o superiore (compatibile nativamente con Debian 12+, Ubuntu 22.04+, Arch Linux, Fedora 36+).
- **Server Grafico**: Compatibile sia con **X11** che con **Wayland**.
- **Desktop Environments**: GNOME, KDE Plasma, XFCE, Cinnamon, MATE, LXQt.

---

## 6. Build, Confezionamento e Distribuzione

### 6.1. Requisiti di Compilazione (Build da Sorgenti)
- **Compilatore**: GCC 11+ o Clang 13+ con supporto C++17/C++20.
- **Sistema di Build**: CMake 3.16 o superiore.
- **Librerie di Sviluppo**:
  - `qt6-base-dev`
  - `qt6-tools-dev`
  - `libgl1-mesa-dev`

Comandi di build:
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### 6.2. Generazione AppImage
L'AppImage viene costruita tramite lo script `packaging/bundle_appimage.py` e il tool `appimagetool`:
- Raccolta delle librerie runtime di Qt 6 necessarie (`libQt6Widgets`, `libQt6Gui`, `libQt6Core`, `libQt6DBus`).
- Inclusione dei plugin di piattaforma Qt (`platforms/libqxcb.so`, `wayland`).
- Generazione dell'`AppRun` per la corretta configurazione dell'ambiente di esecuzione portabile.
- Pacchetto finale prodotto: `FDg-x86_64.AppImage`.

---

## 7. Struttura del Repository

```
FDg/
├── CMakeLists.txt                # Configurazione di build CMake
├── LICENSE                       # Licenza MIT
├── README.md                     # Documentazione per l'utente (IT & EN)
├── SPEC.md                       # Specifiche tecniche e funzionali (questo documento)
├── packaging/
│   ├── build_all.sh              # Script di compilazione completa
│   ├── bundle_appimage.py        # Script Python per il bundling dell'AppImage
│   ├── Dockerfile.debian12       # Ambiente containerizzato di build pulita
│   └── fdg.desktop               # Specifica lanciatore XDG FreeDesktop
├── docs/
│   ├── DOCUMENTAZIONE.md         # Documentazione estesa del progetto
│   ├── FDg-Documentazione.pdf    # Manuale utente e tecnico in formato PDF
│   ├── images/                   # Schermate ufficiali dell'applicazione con privacy blur
│   ├── create_help_files.py      # Generatore delle guide HTML multilingua
│   └── generate_pdf.py           # Generatore della documentazione in PDF
├── resources/
│   ├── resources.qrc             # Qt Resource Collection
│   └── icons/                    # Icone applicazione e bandiere per le lingue
├── src/
│   ├── main.cpp                  # Punto di ingresso dell'applicazione
│   ├── mainwindow.h / .cpp / .ui # Finestra principale e controller GUI
│   ├── fdrunner.h / .cpp         # Wrapper asincrono per il comando fd
│   ├── filemanagerhelper.h / .cpp# Integrazione DBus/CLI per file manager
│   ├── desktopintegrator.h / .cpp# Gestione del lanciatore .desktop di sistema
│   ├── dependencyinstaller.h / .cpp # Installer PolicyKit per il motore fd
│   ├── themehelper.h / .cpp      # Gestione stili Fusion (Chiaro/Scuro) e coerenza visiva
│   └── guidedialog.h / .cpp      # Visualizzatore guida HTML integrata
├── tools/                        # Script ausiliari (cattura screenshot e blur privacy)
└── translations/                 # File di traduzione e localizzazione
```

---

## 8. Manutenzione e Linee Guida per Sviluppi Futuri
1. **Preservare l'Isolamento di PolicyKit**: Non inserire mai prompt per password testuali o chiamate `sudo` dirette; mantenere sempre l'uso di `pkexec` per preservare la sicurezza e la compatibilità desktop.
2. **Mantenere la Portabilità AppImage**: In caso di aggiornamento delle dipendenze C++, verificare sempre che la baseline di GLIBC non superi quella di Debian 12 (GLIBC 2.36) per non compromettere la retrocompatibilità con distribuzioni LTS.
3. **Mantenere Separata la Logica dalla UI**: Tutte le modifiche all'interfaccia grafica devono essere apportate in `src/mainwindow.ui` tramite Qt Designer o codice esplicito nei moduli dedicati, evitando logiche di business all'interno degli handler di visualizzazione.
