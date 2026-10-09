# Istruzioni Operative e Linee Guida per Agenti IA (FDg)

Questo file definisce gli standard architetturali, di conformità e di manutenzione da applicare ad ogni sessione di sviluppo sul repository **FDg**.

---

## 1. Paternità e Crediti
- **Ideazione e Concept**: PsyTech76
- **Ingegnerizzazione e Sviluppo**: Intelligenza Artificiale (Google DeepMind - Advanced Agentic Coding)
- **Licenza**: MIT License (Open Source e Gratuita per la comunità GNU/Linux)

---

## 2. Parità Visiva Cross-Desktop e Coerenza Temi (Style & Theme Invariant)
- **Regola**: L'applicazione deve mantenere una perfetta fedeltà visiva rispetto agli screenshot presenti nella documentazione (`README.md`).
- **Implementazione**:
  - All'avvio in `src/main.cpp`, richiamare sempre `ThemeHelper::initTheme()`.
  - Lo stile predefinito è `Fusion` con palette chiara standard ad alto contrasto (sfondo `#efefef`, base `#ffffff`, testo `#000000`, bottoni in rilievo nitidi).
  - L'applicazione include un selettore tematico dinamico in `Strumenti -> Tema` (*Chiaro*, *Scuro*, *Predefinito di Sistema*), persistito su `QSettings` (`ui/themeMode`).
  - Se vengono generati nuovi screenshot della documentazione, verificare che l'applicazione utilizzi la stessa configurazione tematica.

---

## 3. Privacy e Anonimizzazione nei Documenti e Screenshot
- **Zero Hardcoded Personal Data**: Nessun percorso utente personale (es. `/home/<utente>`) o credenziali deve essere memorizzato o hardcodato.
- **Privacy Blur sugli Screenshot**: Qualsiasi schermata inclusa nel `README.md` o nei manuali che mostri dati simulati sensibili (account utente, IP locali/remoti, credenziali) deve avere applicato un filtro di sfocatura gaussiana (*Privacy Blur*).
- **Documentazione Bilingue**: Tutta la documentazione utente (`README.md`) deve essere strutturata con la sezione Italiana seguita integralmente dalla sezione Inglese, con screenshot contestualizzati nella rispettiva lingua.

---

## 4. Architettura e Sicurezza Runtime
- **Gestione del Processo `fd`**: Esecuzione asincrona con `QProcess`, isolamento del process group (`setpgid`) e kill istantaneo sicuro con `SIGKILL` su richiesta dell'utente.
- **Elevazione Privilegi con PolicyKit**: Per l'installazione delle dipendenze di sistema (`pacman` su Arch, `apt` su Debian), utilizzare esclusivamente `pkexec`. Nessuna memorizzazione di password in chiaro o in memoria.
- **Isolamento Runtime AppImage**: Ripulire l'ambiente (`LD_LIBRARY_PATH`, `QT_PLUGIN_PATH`) tramite `FileManagerHelper::cleanEnvironment()` prima di invocare applicazioni desktop esterne o gestori di pacchetti.
