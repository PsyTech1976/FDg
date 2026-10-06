#!/usr/bin/env python3
import os
import sys
import base64
from PyQt6.QtGui import QGuiApplication, QPdfWriter, QTextDocument, QPageLayout, QPageSize
from PyQt6.QtCore import QMarginsF

def generate_pdf():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.abspath(os.path.join(script_dir, ".."))
    icon_path = os.path.join(project_dir, "resources", "icons", "appicon_256.png")
    output_pdf_docs = os.path.join(script_dir, "FDg-Documentazione.pdf")
    output_pdf_root = os.path.join(project_dir, "FDg-Documentazione.pdf")

    icon_b64 = ""
    if os.path.exists(icon_path):
        with open(icon_path, "rb") as f:
            icon_b64 = base64.b64encode(f.read()).decode("utf-8")

    html_content = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<style>
    body {{
        font-family: 'Liberation Sans', 'DejaVu Sans', sans-serif;
        color: #24292e;
        line-height: 1.45;
        font-size: 10pt;
    }}
    .header-table {{
        width: 100%;
        border-bottom: 2px solid #00f2fe;
        padding-bottom: 10px;
        margin-bottom: 16px;
    }}
    .title {{
        font-size: 22pt;
        font-weight: bold;
        color: #1a1e2d;
        margin: 0;
    }}
    .subtitle {{
        font-size: 12pt;
        color: #0366d6;
        margin-top: 3px;
        margin-bottom: 6px;
    }}
    .badge {{
        background-color: #f1f8ff;
        border: 1px solid #c8e1ff;
        border-radius: 5px;
        padding: 6px 12px;
        color: #0366d6;
        font-weight: bold;
        font-size: 9.5pt;
        display: inline-block;
    }}
    h2 {{
        color: #1a1e2d;
        border-bottom: 1.5px solid #eaecef;
        padding-bottom: 4px;
        margin-top: 18px;
        margin-bottom: 8px;
        font-size: 13.5pt;
    }}
    h3 {{
        color: #24292e;
        margin-top: 12px;
        margin-bottom: 4px;
        font-size: 11.5pt;
    }}
    table.data-table {{
        width: 100%;
        border-collapse: collapse;
        margin: 10px 0;
        font-size: 9.5pt;
    }}
    table.data-table th {{
        background-color: #f6f8fa;
        color: #24292e;
        font-weight: bold;
        text-align: left;
        padding: 6px 8px;
        border: 1px solid #d1d5da;
    }}
    table.data-table td {{
        padding: 6px 8px;
        border: 1px solid #d1d5da;
        vertical-align: top;
    }}
    .callout {{
        background-color: #e6ffed;
        border-left: 4px solid #28a745;
        padding: 8px 12px;
        margin: 10px 0;
        border-radius: 3px;
    }}
    pre, code {{
        font-family: 'Liberation Mono', 'DejaVu Sans Mono', monospace;
        font-size: 9pt;
        background-color: #f6f8fa;
    }}
    pre {{
        padding: 8px;
        border-radius: 4px;
        border: 1px solid #e1e4e8;
        line-height: 1.3;
    }}
    ul, ol {{
        margin-top: 3px;
        margin-bottom: 6px;
        padding-left: 18px;
    }}
    li {{
        margin-bottom: 3px;
    }}
    .footer-note {{
        margin-top: 22px;
        border-top: 1px solid #eaecef;
        padding-top: 8px;
        font-size: 8.5pt;
        color: #586069;
        text-align: center;
    }}
</style>
</head>
<body>

<table class="header-table">
<tr>
    <td style="width: 70px; vertical-align: middle;">
        <img src="data:image/png;base64,{icon_b64}" width="64" height="64" />
    </td>
    <td style="vertical-align: middle; padding-left: 12px;">
        <div class="title">FDg v1.2.0</div>
        <div class="subtitle">Compatibilità Universale Debian & Arch Linux</div>
        <div class="badge">Applicazione realizzata con IA su un'idea di PsyTech76 - Gratuita</div>
    </td>
</tr>
</table>

<h2>1. Panoramica e Compatibilità Cross-Distribution</h2>
<p>
<b>FDg</b> è un'applicazione grafica per ambienti GNU/Linux basata sul framework <b>Qt 6</b> e <b>C++20</b>. 
L'applicazione è progettata per essere compatibile al 100% sia con le distribuzioni basate su <b>Debian</b> (Debian 12 Bookworm, Debian 13 Trixie, Ubuntu 22.04/24.04, Linux Mint, Pop!_OS) sia con le distribuzioni basate su <b>Arch Linux</b> (Arch, CachyOS, Manjaro, EndeavourOS) e Fedora.
</p>

<div class="callout">
<b>Punti di forza dell'architettura universale:</b>
<ul>
    <li><b>Riconoscimento automatico comando:</b> Rileva automaticamente sia <code>fd</code> (standard su Arch/Fedora) sia <code>fdfind</code> (standard su Debian/Ubuntu), oltre a supportare la variabile <code>FD_PATH</code>.</li>
    <li><b>Portabilità AppImage universale:</b> Il pacchetto è generato con un target <b>GLIBC 2.34+</b>, garantendo l'esecuzione immediata sia su Debian 12 Stabile che sui più recenti sistemi rolling-release Arch Linux.</li>
    <li><b>Integrazione Desktop completa:</b> Supporta DBus <code>FileManager1</code>, Dolphin (KDE), Nautilus (GNOME), Nemo (Cinnamon), Thunar (XFCE) e PCManFM (LXQt/LXDE).</li>
    <li><b>Isolamento ambiente AppImage:</b> Nessun conflitto di librerie all'apertura dei file associati di sistema (Kate, VLC, Gedit).</li>
</ul>
</div>

<h2>2. Tabella di Conformità ai Requisiti</h2>
<table class="data-table">
    <tr>
        <th style="width: 30%;">Requisito</th>
        <th style="width: 12%; text-align: center;">Stato</th>
        <th>Dettagli Implementativi</th>
    </tr>
    <tr>
        <td><b>Compatibilità Debian & Arch</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Rilevamento <code>fd</code> e <code>fdfind</code>; AppImage testata sia su Debian 12 che su Arch Linux.</td>
    </tr>
    <tr>
        <td><b>Frontend GUI per fd</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Processo <code>fd</code>/<code>fdfind</code> gestito in modo asincrono tramite <code>QProcess</code> con batching a 40ms.</td>
    </tr>
    <tr>
        <td><b>Avvio e Kill Immediato</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Campo di testo e pulsante dinamico. Kill forzato (SIGKILL) a processo e process group. Sicuro al 100% per l'OS (read-only).</td>
    </tr>
    <tr>
        <td><b>Default Root e Tasto Home</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Percorso predefinito impostato su <code>/</code> e pulsante Home per selezionare la cartella utente con un click.</td>
    </tr>
    <tr>
        <td><b>Jolly MS-DOS (*, ?)</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Opzione <code>--glob</code> di <code>fd</code> con checkbox dedicata (supporta es. <code>nome*.jpg</code>).</td>
    </tr>
    <tr>
        <td><b>Guida HTML & Browser Integrato</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Guida HTML in 5 lingue integrata nell'AppImage, visualizzabile con browser minimale dedicato (F1 / footer).</td>
    </tr>
    <tr>
        <td><b>Bandiere Lingua Grafiche</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Icone delle bandiere (SVG/PNG) per visualizzazione nitida e perfetta su qualsiasi desktop e font Linux.</td>
    </tr>
    <tr>
        <td><b>List box a 2 colonne</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td><code>QTreeWidget</code> (<i>Nome File</i> e <i>Percorso</i>) ordinato inizialmente per coerenza/scoperta.</td>
    </tr>
    <tr>
        <td><b>Ordinamento colonne</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Click sull'intestazione per ordinare in modo naturale (crescente A-Z 0-9 e decrescente).</td>
    </tr>
    <tr>
        <td><b>Barra di avanzamento</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Animazione continua durante la scansione con contatore in tempo reale ed esito finale.</td>
    </tr>
    <tr>
        <td><b>Doppio click sul path</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Apre il file manager ed <b>evidenzia il file selezionato senza aprirlo</b> tramite DBus o comando diretto.</td>
    </tr>
    <tr>
        <td><b>Doppio click nome file</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Apre direttamente il file con l'applicazione associata in ambiente isolato (senza crash).</td>
    </tr>
    <tr>
        <td><b>Attribution PsyTech76</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Banner nel footer: <i>"Applicazione realizzata con IA su un'idea di PsyTech76 - Gratuita"</i>.</td>
    </tr>
    <tr>
        <td><b>Qt 6 C++ con UI separata</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Interfaccia in <code>mainwindow.ui</code> (Qt Designer XML) e codice C++20 commentato in italiano.</td>
    </tr>
    <tr>
        <td><b>Supporto multi-lingua</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Italiano, Inglese, Tedesco, Spagnolo e Francese commutabili a caldo dall'interfaccia.</td>
    </tr>
    <tr>
        <td><b>Validazione Ricerca Vuota</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Pulsante disabilitato se il campo cerca è vuoto; blocco Invio con avviso nella barra di stato.</td>
    </tr>
    <tr>
        <td><b>Visualizzazione Directory</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Normalizzazione slash finale nei percorsi: nome cartella e percorso genitore sempre corretti.</td>
    </tr>
    <tr>
        <td><b>Nuova Ricerca & Reset</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Menu File / Ctrl+N: arresto scansioni, azzeramento testo e pulizia integrale risultati precedenti.</td>
    </tr>
    <tr>
        <td><b>Parametri Collassabili</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Opzioni avanzate e lingua a scomparsa con triangolino cliccabile (▶/▼), persistente con QSettings.</td>
    </tr>
    <tr>
        <td><b>Integrazione Desktop Automatica</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Menu Strumenti: installazione e rimozione lanciatore .desktop per blocco permanente su barra/dock.</td>
    </tr>
    <tr>
        <td><b>Verifica & Installazione Motore 'fd'</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Verifica preventiva all'avvio e da menu; se presente conferma versione senza reinstallare; solo se assente installa.</td>
    </tr>
    <tr>
        <td><b>Archivio compresso Desktop</b></td>
        <td style="text-align: center; color: green; font-weight: bold;">Conforme</td>
        <td>Archivio <code>.zip</code> salvato su Desktop e Scrivania contenente solo AppImage e documentazione.</td>
    </tr>
</table>

<h2>3. Guida all'Uso e Esempi di Sintassi</h2>
<ul>
    <li><code>*.txt</code> : trova tutti i file con estensione <code>.txt</code></li>
    <li><code>*.*</code> : trova qualsiasi file dotato di estensione (esattamente come su MS-DOS)</li>
    <li><code>doc*</code> : trova tutti i file e cartelle il cui nome inizia per "doc"</li>
    <li><code>*progetto*</code> : trova tutti gli elementi che contengono la parola "progetto"</li>
    <li><code>test?.png</code> : trova <code>test1.png</code>, <code>test2.png</code>, ecc.</li>
    <li><code>*</code> : elenca tutti gli elementi presenti nella directory selezionata</li>
    <li><b>Doppio click su Nome File:</b> apre il file con l'applicazione predefinita (es. Kate, VLC, Gedit, ecc.)</li>
    <li><b>Doppio click su Percorso:</b> apre il gestore file evidenziando l'elemento senza aprirlo</li>
    <li><b>Tasto Interrompi:</b> arresta istantaneamente la ricerca in corso</li>
    <li><b>Ctrl+N (Nuova Ricerca):</b> cancella il testo e rimuove completamente i risultati precedenti</li>
    <li><b>Menu Strumenti &rarr; Integra nel sistema:</b> integra o rimuove FDg dal menu di sistema e dalla barra delle applicazioni</li>
    <li><b>Menu Strumenti &rarr; Verifica o Installa motore 'fd'...:</b> controlla presenza e versione del motore fd; installa solo in sua assenza</li>
</ul>

<h2>4. Note Tecniche per Future Revisioni (Umano / IA)</h2>
<ul>
    <li><b>Bandiere grafiche i18n:</b> Implementate con icone SVG/PNG (<code>resources/icons/flags/</code>) caricate in <code>QIcon</code> per eliminare dipendenze da font emoji colore di sistema e garantire resa impeccabile.</li>
    <li><b>Kill immediato e sicurezza SIGKILL:</b> <code>fd</code> effettua solo scansioni in lettura (read-only); l'invio forzato di <code>SIGKILL</code> al processo e al process group (<code>setpgid</code>) è sicuro al 100% per OS e file, azzerando istantaneamente l'uso di CPU/RAM.</li>
    <li><b>Percorso di default e tasto Home:</b> Root del disco (<code>/</code>) impostata come cartella predefinita e pulsante dedicato <b>Home</b> per passare alla directory personale con un click.</li>
    <li><b>Guida HTML & Browser integrato:</b> Incorporata nelle risorse binarie (AppImage autonoma al 100%) e visualizzata con browser minimale leggero nativo <code>QTextBrowser</code> (senza i 150MB di Chromium).</li>
    <li><b>Ergonomia & Isolamento:</b> Attivazione solo su doppio click per prevenire aperture con singolo click su KDE Plasma; ambiente ripulito da <code>LD_LIBRARY_PATH</code> all'apertura di file esterni.</li>
    <li><b>Normalizzazione percorsi cartelle:</b> Quando <code>fd</code> individua una cartella termina con uno slash (<code>/</code>), facendo restituire stringa vuota a <code>QFileInfo::fileName()</code>; la normalizzazione toglie gli slash finali estraendo sempre il nome reale della cartella.</li>
    <li><b>Disabilitazione ricerca a campo vuoto:</b> Il pulsante Cerca si attiva dinamicamente solo in presenza di testo (anche con spazi esclusi) e la pressione di Invio su campo vuoto è inibita con messaggio di avviso.</li>
    <li><b>Reset Completo Nuova Ricerca:</b> L'attivazione di Nuova Ricerca (Ctrl+N o menu) azzera lo stato dell'applicazione, disabilita il pulsante Cerca e svuota la tabella dei risultati.</li>
    <li><b>Pannello Parametri Collassabile:</b> Widget a scomparsa con indicatore a triangolo (▶/▼) che racchiude i filtri avanzati e il selettore lingua, salvando la configurazione in QSettings per avvii puliti.</li>
    <li><b>Integrazione Desktop Automatica (DesktopIntegrator):</b> Risolve l'effimera persistenza delle AppImage su KDE Plasma/GNOME (cartelle <code>/tmp/.mount_XXXXXX/</code> smontate all'uscita); registra in <code>~/.local/share/applications/fdg.desktop</code> il percorso permanente e l'icona con rilevamento <code>APPIMAGE</code>, consentendo il blocco permanente alla barra.</li>
    <li><b>Verifica preventiva e installazione motore 'fd' (DependencyInstallerDialog):</b> Verifica preliminare all'avvio e da menu dell'eseguibile <code>fd</code> o <code>fdfind</code>; se già presente riporta percorso e versione senza procedere a reinstallazioni ridondanti. Solo in caso di effettiva assenza propone l'installazione guidata multi-distro (Arch con <code>pacman</code>, Debian/Ubuntu con <code>apt-get</code>, Fedora con <code>dnf</code>, openSUSE con <code>zypper</code>) con elevazione privilegi PolicyKit (<code>pkexec</code>), isolamento ambiente AppImage (<code>cleanEnvironment</code>), log in tempo reale ANSI-clean e notifica finale di successo.</li>
</ul>

<div class="footer-note">
    Progetto realizzato per la comunità GNU/Linux | Ideazione: <b>PsyTech76</b> | Realizzazione: <b>Assistente IA</b> | Licenza Gratuita
</div>

</body>
</html>
"""

    app = QGuiApplication(sys.argv)
    
    for out_path in [output_pdf_docs, output_pdf_root]:
        writer = QPdfWriter(out_path)
        writer.setPageLayout(QPageLayout(
            QPageSize(QPageSize.PageSizeId.A4),
            QPageLayout.Orientation.Portrait,
            QMarginsF(12, 12, 12, 12)
        ))
        writer.setTitle("FDg v1.2.0 - Documentazione Tecnica")
        writer.setCreator("FDg - PsyTech76 & IA")
        
        doc = QTextDocument()
        doc.setHtml(html_content)
        doc.print(writer)
        print(f"PDF generato con successo: {out_path} ({os.path.getsize(out_path)} bytes)")

if __name__ == "__main__":
    generate_pdf()
