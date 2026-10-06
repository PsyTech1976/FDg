import os

script_dir = os.path.dirname(os.path.abspath(__file__))
help_dir = os.path.abspath(os.path.join(script_dir, "..", "resources", "help"))
os.makedirs(help_dir, exist_ok=True)

css = """
body {
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Liberation Sans", sans-serif;
    color: #24292e;
    line-height: 1.5;
    margin: 16px;
    background-color: #ffffff;
}
.lang-bar {
    background-color: #f6f8fa;
    border: 1px solid #d1d5da;
    border-radius: 6px;
    padding: 8px 12px;
    margin-bottom: 16px;
}
.lang-title {
    font-weight: bold;
    color: #444d56;
    margin-right: 8px;
}
.lang-link {
    text-decoration: none;
    color: #0366d6;
    padding: 4px 8px;
    border-radius: 4px;
    font-weight: 500;
}
.lang-link:hover {
    background-color: #e1e4e8;
}
.lang-link.active {
    background-color: #0366d6;
    color: #ffffff;
    font-weight: bold;
}
h1 {
    color: #1a1e2d;
    border-bottom: 2px solid #00f2fe;
    padding-bottom: 8px;
    margin-top: 0;
}
h2 {
    color: #1a1e2d;
    border-bottom: 1px solid #eaecef;
    padding-bottom: 5px;
    margin-top: 24px;
}
h3 {
    color: #24292e;
    margin-top: 16px;
}
.callout {
    background-color: #f1f8ff;
    border-left: 4px solid #0366d6;
    padding: 10px 14px;
    margin: 12px 0;
    border-radius: 3px;
}
.callout-success {
    background-color: #dcffe4;
    border-left: 4px solid #28a745;
}
.callout-warning {
    background-color: #fffbdd;
    border-left: 4px solid #f66a0a;
}
code {
    background-color: #f6f8fa;
    border: 1px solid #e1e4e8;
    border-radius: 3px;
    padding: 2px 5px;
    font-family: "Liberation Mono", monospace;
    font-size: 90%;
    color: #d73a49;
}
table {
    border-collapse: collapse;
    width: 100%;
    margin: 12px 0;
}
th, td {
    border: 1px solid #d1d5da;
    padding: 8px 10px;
    text-align: left;
}
th {
    background-color: #f6f8fa;
}
ul, ol {
    padding-left: 20px;
}
li {
    margin-bottom: 4px;
}
.footer {
    margin-top: 30px;
    padding-top: 12px;
    border-top: 1px solid #eaecef;
    font-size: 85%;
    color: #586069;
    text-align: center;
}
"""

def make_lang_bar(curr):
    langs = [
        ("it", "guide_it.html", "🇮🇹 Italiano"),
        ("en", "guide_en.html", "🇬🇧 English"),
        ("de", "guide_de.html", "🇩🇪 Deutsch"),
        ("es", "guide_es.html", "🇪🇸 Español"),
        ("fr", "guide_fr.html", "🇫🇷 Français")
    ]
    links = []
    for code, fn, label in langs:
        act = " active" if code == curr else ""
        links.append(f"<a href=\"{fn}\" class=\"lang-link{act}\">{label}</a>")
    return "<div class=\"lang-bar\"><span class=\"lang-title\">🌐 Lingua / Language:</span> " + " &nbsp;|&nbsp; ".join(links) + "</div>"

it_html = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>FDg - Manuale Utente</title>
<style>{css}</style>
</head>
<body>
{make_lang_bar("it")}

<h1>📖 Guida Utente di FDg v1.2.0</h1>
<p><i>Applicazione realizzata con IA su un'idea di PsyTech76 - Gratuita</i></p>

<h2>1. Cos'è FDg?</h2>
<p>
<b>FDg</b> è un'interfaccia grafica moderna, reattiva e semplice da usare per il potentissimo comando di ricerca Linux <code>fd</code> (distribuito come <code>fdfind</code> su Debian e Ubuntu).
FDg permette di trovare all'istante file e directory su qualsiasi volume del disco, con supporto completo ai caratteri jolly stile MS-DOS (<code>*</code> e <code>?</code>), ordinamento naturale e piena integrazione con il desktop.
</p>

<h2>2. Come avviare una ricerca</h2>
<ol>
    <li><b>Campo Cerca:</b> inserisci il nome del file o il modello da cercare (es. <code>documento*.pdf</code> o <code>*report*</code>).</li>
    <li><b>Cartella di partenza:</b>
        <ul>
            <li>Per impostazione predefinita è impostata la radice del sistema <code>/</code>.</li>
            <li>Premi il pulsante <b>Home</b> per impostare istantaneamente la tua cartella personale utente.</li>
            <li>Premi <b>Sfoglia...</b> per selezionare qualsiasi altra cartella grafica.</li>
        </ul>
    </li>
    <li><b>Filtri ed opzioni:</b>
        <ul>
            <li><b>File nascosti (-H):</b> cerca anche tra file e cartelle nascoste (che iniziano con <code>.</code>).</li>
            <li><b>Distingui maiuscole (-s):</b> esegue una ricerca case-sensitive.</li>
            <li><b>Segui symlink (-L):</b> esplora anche i collegamenti simbolici.</li>
            <li><b>Usa caratteri jolly (* e ?):</b> abilitato di default, permette l'uso di pattern MS-DOS.</li>
            <li><b>Tipo elemento:</b> limita la ricerca a <i>Tutti gli elementi</i>, <i>Solo file</i> o <i>Solo cartelle</i>.</li>
        </ul>
    </li>
    <li>Premi il pulsante <b>Cerca</b> oppure premi <b>Invio</b> dalla tastiera.</li>
</ol>

<h2>3. Caratteri Jolly MS-DOS (* e ?)</h2>
<p>FDg supporta in modo nativo la sintassi dei caratteri jolly stile DOS/Glob:</p>
<table>
    <tr><th>Esempio</th><th>Descrizione</th></tr>
    <tr><td><code>*.txt</code></td><td>Tutti i file con estensione .txt</td></tr>
    <tr><td><code>*.*</code></td><td>Tutti i file dotati di una qualsiasi estensione</td></tr>
    <tr><td><code>nome*.jpg</code></td><td>File che iniziano con 'nome' e terminano con '.jpg'</td></tr>
    <tr><td><code>doc*</code></td><td>File e cartelle il cui nome inizia con 'doc'</td></tr>
    <tr><td><code>*bilancio*</code></td><td>File contenenti la parola 'bilancio' ovunque nel nome</td></tr>
    <tr><td><code>foto?.png</code></td><td>Sostituisce un singolo carattere (es. foto1.png, fotoA.png)</td></tr>
    <tr><td><code>*</code></td><td>Mostra tutti gli elementi presenti nella cartella selezionata</td></tr>
</table>

<h2>4. Annullamento e Chiusura Immediata del Processo (Kill App)</h2>
<div class="callout callout-success">
<b>Sicurezza e Performance Garantite:</b><br/>
Quando premi il pulsante <b>Interrompi</b>, il processo <code>fd</code> in background viene terminato all'istante tramite <code>SIGKILL</code>.<br/>
<b>Perché è sicuro al 100%?</b> Il comando <code>fd</code> effettua esclusivamente operazioni di lettura (read-only) sui metadati del filesystem. Non scrive, modifica né cancella mai alcun file. La chiusura forzata istantanea è quindi completamente sicura sia per il sistema operativo che per tutti i tuoi documenti, garantendo al contempo la liberazione immediata della memoria e del processore (CPU).
</div>

<h2>5. Risultati e Interazione</h2>
<ul>
    <li><b>Doppio click sul Nome File:</b> apre immediatamente il file con l'applicazione predefinita di sistema (Kate, VLC, Gedit, LibreOffice, Visualizzatore immagini, ecc.).</li>
    <li><b>Doppio click sul Percorso:</b> apre il gestore file di sistema (Dolphin, Nautilus, Nemo, Thunar, PCManFM) ed <b>evidenzia il file selezionato senza aprirlo</b>.</li>
    <li><b>Click sull'intestazione colonna:</b> riordina la lista in ordine crescente (A-Z / 0-9) o decrescente.</li>
    <li><b>Tasto Destro (Menu Contestuale):</b> opzioni per aprire il file, aprirlo nel gestore file, copiare il percorso assoluto o copiare il nome negli appunti.</li>
</ul>

<h2>6. Scorciatoie da tastiera</h2>
<table>
    <tr><th>Scorciatoia</th><th>Azione</th></tr>
    <tr><td><code>Ctrl+N</code></td><td>Pulisce il campo cerca e prepara una nuova ricerca</td></tr>
    <tr><td><code>Ctrl+Q</code></td><td>Chiude l'applicazione</td></tr>
    <tr><td><code>F1</code></td><td>Apre questa guida utente</td></tr>
    <tr><td><code>Invio</code> nel campo cerca</td><td>Avvia la ricerca</td></tr>
    <tr><td><code>Invio</code> sulla lista risultati</td><td>Apre il file selezionato</td></tr>
</table>

<div class="footer">
FDg v1.2.0 | Ideazione: PsyTech76 | Realizzazione con IA | Software Gratuito Open Source
</div>
</body>
</html>
"""

en_html = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>FDg - User Guide</title>
<style>{css}</style>
</head>
<body>
{make_lang_bar("en")}

<h1>📖 FDg User Guide v1.2.0</h1>
<p><i>Application created with AI based on an idea by PsyTech76 - Free & Open</i></p>

<h2>1. What is FDg?</h2>
<p>
<b>FDg</b> is a modern, responsive, and intuitive graphical interface for the blazing fast Linux search utility <code>fd</code> (packaged as <code>fdfind</code> on Debian and Ubuntu).
FDg allows you to instantly locate files and folders anywhere on your system, with full support for MS-DOS style wildcards (<code>*</code> and <code>?</code>), natural sorting, and seamless desktop integration.
</p>

<h2>2. How to Search</h2>
<ol>
    <li><b>Search Field:</b> Enter the file name or pattern to look for (e.g. <code>document*.pdf</code> or <code>*report*</code>).</li>
    <li><b>Starting Folder:</b>
        <ul>
            <li>Defaults to the root filesystem <code>/</code>.</li>
            <li>Click the <b>Home</b> button to instantly set your user home directory.</li>
            <li>Click <b>Browse...</b> to pick any folder using the directory picker.</li>
        </ul>
    </li>
    <li><b>Filters and Options:</b>
        <ul>
            <li><b>Hidden files (-H):</b> Search hidden files and directories (names starting with <code>.</code>).</li>
            <li><b>Case sensitive (-s):</b> Case-sensitive search.</li>
            <li><b>Follow symlinks (-L):</b> Traverse symbolic links.</li>
            <li><b>Use wildcards (* and ?):</b> Enabled by default, allows DOS/Glob patterns.</li>
            <li><b>Type:</b> Restrict to <i>All items</i>, <i>Files only</i>, or <i>Folders only</i>.</li>
        </ul>
    </li>
    <li>Click <b>Search</b> or press <b>Enter</b> on your keyboard.</li>
</ol>

<h2>3. MS-DOS Wildcard Patterns (* and ?)</h2>
<table>
    <tr><th>Pattern</th><th>Description</th></tr>
    <tr><td><code>*.txt</code></td><td>All files ending with .txt extension</td></tr>
    <tr><td><code>*.*</code></td><td>Any file having an extension</td></tr>
    <tr><td><code>name*.jpg</code></td><td>Files beginning with 'name' and ending in '.jpg'</td></tr>
    <tr><td><code>doc*</code></td><td>Files and folders beginning with 'doc'</td></tr>
    <tr><td><code>*report*</code></td><td>Files containing 'report' anywhere in the name</td></tr>
    <tr><td><code>photo?.png</code></td><td>Matches single character placeholder (e.g. photo1.png, photoA.png)</td></tr>
    <tr><td><code>*</code></td><td>Lists all items in the chosen folder</td></tr>
</table>

<h2>4. Immediate Search Cancellation (Kill App / Process)</h2>
<div class="callout callout-success">
<b>Guaranteed Safety and Performance:</b><br/>
When you click <b>Cancel</b>, the background <code>fd</code> process is immediately terminated via <code>SIGKILL</code>.<br/>
<b>Why is this 100% safe?</b> The <code>fd</code> utility exclusively performs read-only filesystem scanning. It never writes to, modifies, or deletes files. Forcefully killing it is completely safe for your operating system and documents, while instantly releasing CPU and RAM resources.
</div>

<h2>5. Results and Actions</h2>
<ul>
    <li><b>Double-click File Name:</b> Immediately opens the file using your default desktop application (Kate, VLC, Gedit, LibreOffice, image viewer, etc.).</li>
    <li><b>Double-click Path:</b> Opens your system file manager (Dolphin, Nautilus, Nemo, Thunar, PCManFM) and <b>highlights the selected file without opening it</b>.</li>
    <li><b>Click Column Header:</b> Sorts results alphabetically and numerically in ascending (A-Z / 0-9) or descending order.</li>
    <li><b>Right-click (Context Menu):</b> Options to open file, show in file manager, copy absolute path, or copy file name.</li>
</ul>

<h2>6. Keyboard Shortcuts</h2>
<table>
    <tr><th>Shortcut</th><th>Action</th></tr>
    <tr><td><code>Ctrl+N</code></td><td>Clears the query field for a new search</td></tr>
    <tr><td><code>Ctrl+Q</code></td><td>Exits the application</td></tr>
    <tr><td><code>F1</code></td><td>Opens this User Guide</td></tr>
    <tr><td><code>Enter</code> in search box</td><td>Starts the search</td></tr>
    <tr><td><code>Enter</code> on results list</td><td>Opens the selected file</td></tr>
</table>

<div class="footer">
FDg v1.2.0 | Concept: PsyTech76 | Developed with AI | Free Open Source Software
</div>
</body>
</html>
"""

de_html = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>FDg - Benutzerhandbuch</title>
<style>{css}</style>
</head>
<body>
{make_lang_bar("de")}

<h1>📖 FDg Benutzerhandbuch v1.2.0</h1>
<p><i>Anwendung erstellt mit KI nach einer Idee von PsyTech76 - Kostenlos</i></p>

<h2>1. Was ist FDg?</h2>
<p>
<b>FDg</b> ist eine moderne, reaktionsschnelle und benutzerfreundliche grafische Oberfläche für den extrem schnellen Linux-Suchbefehl <code>fd</code> (auf Debian/Ubuntu als <code>fdfind</code> bezeichnet).
FDg findet Dateien und Ordner im Handumdrehen auf jedem Datenträger, mit voller Unterstützung für MS-DOS-Platzhalter (<code>*</code> und <code>?</code>), natürlicher Sortierung und vollständiger Desktop-Integration.
</p>

<h2>2. Durchführung einer Suche</h2>
<ol>
    <li><b>Suchfeld:</b> Geben Sie den Dateinamen oder das Suchmuster ein (z. B. <code>dokument*.pdf</code> oder <code>*bericht*</code>).</li>
    <li><b>Startordner:</b>
        <ul>
            <li>Standardmäßig auf das Root-Verzeichnis <code>/</code> eingestellt.</li>
            <li>Klicken Sie auf <b>Home</b>, um sofort Ihr Benutzerverzeichnis festzulegen.</li>
            <li>Klicken Sie auf <b>Durchsuchen...</b>, um einen beliebigen Ordner auszuwählen.</li>
        </ul>
    </li>
    <li><b>Filter und Optionen:</b>
        <ul>
            <li><b>Versteckte Dateien (-H):</b> Sucht auch in versteckten Dateien und Verzeichnissen.</li>
            <li><b>Groß-/Kleinschreibung (-s):</b> Berücksichtigt Groß- und Kleinschreibung.</li>
            <li><b>Symlinks folgen (-L):</b> Folgt symbolischen Verknüpfungen.</li>
            <li><b>Platzhalter (* und ?):</b> Standardmäßig aktiv für DOS-Muster.</li>
            <li><b>Elementtyp:</b> Beschränkung auf <i>Alle Elemente</i>, <i>Nur Dateien</i> oder <i>Nur Ordner</i>.</li>
        </ul>
    </li>
    <li>Klicken Sie auf <b>Suchen</b> oder drücken Sie <b>Eingabe</b> auf der Tastatur.</li>
</ol>

<h2>3. MS-DOS Platzhalter (* und ?)</h2>
<table>
    <tr><th>Muster</th><th>Beschreibung</th></tr>
    <tr><td><code>*.txt</code></td><td>Alle Dateien mit der Endung .txt</td></tr>
    <tr><td><code>*.*</code></td><td>Alle Dateien mit einer beliebigen Erweiterung</td></tr>
    <tr><td><code>name*.jpg</code></td><td>Dateien, die mit 'name' beginnen und auf '.jpg' enden</td></tr>
    <tr><td><code>doc*</code></td><td>Dateien und Ordner, die mit 'doc' beginnen</td></tr>
    <tr><td><code>*bericht*</code></td><td>Dateien, die 'bericht' im Namen enthalten</td></tr>
    <tr><td><code>test?.png</code></td><td>Ersetzt ein einzelnes Zeichen (z. B. test1.png)</td></tr>
    <tr><td><code>*</code></td><td>Listet alle Elemente im gewählten Ordner auf</td></tr>
</table>

<h2>4. Sofortiger Suchabbruch (Kill App / Prozess)</h2>
<div class="callout callout-success">
<b>Sicherheit und Performance garantiert:</b><br/>
Wenn Sie auf <b>Abbrechen</b> klicken, wird der Hintergrundprozess <code>fd</code> sofort mit <code>SIGKILL</code> beendet.<br/>
<b>Warum ist das absolut sicher?</b> Der Befehl <code>fd</code> führt ausschließlich Leseoperationen (read-only) auf Dateisystem-Metadaten durch. Es werden niemals Dateien verändert oder gelöscht. Das sofortige Beenden ist daher völlig sicher für Ihr System und gibt Prozessor (CPU) und Arbeitsspeicher unmittelbar frei.
</div>

<h2>5. Ergebnisse und Interaktion</h2>
<ul>
    <li><b>Doppelklick auf Dateiname:</b> Öffnet die Datei direkt mit der Standardanwendung Ihres Systems.</li>
    <li><b>Doppelklick auf Pfad:</b> Öffnet den Dateimanager (Dolphin, Nautilus usw.) und <b>hebt die Datei hervor, ohne sie zu öffnen</b>.</li>
    <li><b>Klick auf Spaltenkopf:</b> Sortiert die Liste aufsteigend oder absteigend.</li>
    <li><b>Rechtsklick (Kontextmenü):</b> Optionen zum Öffnen, Anzeigen im Dateimanager oder Kopieren des Pfads/Namens.</li>
</ul>

<div class="footer">
FDg v1.2.0 | Idee: PsyTech76 | Entwickelt mit KI | Kostenlose Open-Source-Software
</div>
</body>
</html>
"""

es_html = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>FDg - Guía del Usuario</title>
<style>{css}</style>
</head>
<body>
{make_lang_bar("es")}

<h1>📖 Guía del Usuario de FDg v1.2.0</h1>
<p><i>Aplicación realizada con IA sobre una idea de PsyTech76 - Gratuita</i></p>

<h2>1. ¿Qué es FDg?</h2>
<p>
<b>FDg</b> es una interfaz gráfica moderna, rápida e intuitiva para la potente herramienta de búsqueda de Linux <code>fd</code> (llamada <code>fdfind</code> en Debian y Ubuntu).
FDg le permite encontrar archivos y carpetas al instante, con soporte completo para comodines estilo MS-DOS (<code>*</code> y <code>?</code>), ordenación natural e integración total con el escritorio.
</p>

<h2>2. Cómo realizar una búsqueda</h2>
<ol>
    <li><b>Campo Buscar:</b> Ingrese el nombre del archivo o patrón (ej. <code>documento*.pdf</code> o <code>*informe*</code>).</li>
    <li><b>Carpeta de inicio:</b>
        <ul>
            <li>Por defecto está configurada la raíz del disco <code>/</code>.</li>
            <li>Haga clic en <b>Home</b> para establecer al instante su carpeta personal de usuario.</li>
            <li>Haga clic en <b>Examinar...</b> para seleccionar cualquier otra carpeta.</li>
        </ul>
    </li>
    <li><b>Filtros y opciones:</b>
        <ul>
            <li><b>Archivos ocultos (-H):</b> Busca también en archivos ocultos.</li>
            <li><b>Distinguir mayúsculas (-s):</b> Búsqueda sensible a mayúsculas.</li>
            <li><b>Seguir enlaces simbólicos (-L):</b> Sigue enlaces simbólicos.</li>
            <li><b>Comodines (* y ?):</b> Activo por defecto para patrones MS-DOS.</li>
            <li><b>Tipo de elemento:</b> <i>Todos los elementos</i>, <i>Solo archivos</i> o <i>Solo carpetas</i>.</li>
        </ul>
    </li>
    <li>Haga clic en <b>Buscar</b> o presione <b>Intro</b> en el teclado.</li>
</ol>

<h2>3. Comodines estilo MS-DOS (* y ?)</h2>
<table>
    <tr><th>Ejemplo</th><th>Descripción</th></tr>
    <tr><td><code>*.txt</code></td><td>Todos los archivos con extensión .txt</td></tr>
    <tr><td><code>*.*</code></td><td>Cualquier archivo con alguna extensión</td></tr>
    <tr><td><code>nombre*.jpg</code></td><td>Archivos que comienzan por 'nombre' y terminan en '.jpg'</td></tr>
    <tr><td><code>doc*</code></td><td>Archivos y carpetas que comienzan por 'doc'</td></tr>
    <tr><td><code>*informe*</code></td><td>Archivos que contienen 'informe' en su nombre</td></tr>
    <tr><td><code>foto?.png</code></td><td>Sustituye un solo carácter (ej. foto1.png, fotoA.png)</td></tr>
    <tr><td><code>*</code></td><td>Muestra todos los elementos en la carpeta seleccionada</td></tr>
</table>

<h2>4. Cancelación Inmediata de Búsqueda (Kill App / Proceso)</h2>
<div class="callout callout-success">
<b>Seguridad y Rendimiento Garantizados:</b><br/>
Al pulsar <b>Detener</b>, el proceso <code>fd</code> en segundo plano se finaliza al instante mediante <code>SIGKILL</code>.<br/>
<b>¿Por qué es 100% seguro?</b> La herramienta <code>fd</code> realiza exclusivamente lecturas (read-only) de los metadatos del sistema de archivos. Nunca modifica, borra ni escribe datos. Finalizarlo de inmediato es completamente seguro para su sistema operativo y documentos, liberando al instante la CPU y la memoria RAM.
</div>

<h2>5. Resultados y Acciones</h2>
<ul>
    <li><b>Doble clic en Nombre de archivo:</b> Abre de inmediato el archivo con la aplicación predeterminada de su sistema.</li>
    <li><b>Doble clic en Ruta:</b> Abre el administrador de archivos (Dolphin, Nautilus, etc.) y <b>resalta el archivo seleccionado sin abrirlo</b>.</li>
    <li><b>Clic en cabecera de columna:</b> Ordena la lista de forma ascendente o descendente.</li>
    <li><b>Clic derecho (Menú contextual):</b> Opciones para abrir el archivo, mostrar en carpeta o copiar ruta/nombre.</li>
</ul>

<div class="footer">
FDg v1.2.0 | Idea: PsyTech76 | Desarrollado con IA | Software Gratuito Open Source
</div>
</body>
</html>
"""

fr_html = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>FDg - Guide de l'utilisateur</title>
<style>{css}</style>
</head>
<body>
{make_lang_bar("fr")}

<h1>📖 Guide de l'utilisateur FDg v1.2.0</h1>
<p><i>Application réalisée avec l'IA sur une idée de PsyTech76 - Gratuite</i></p>

<h2>1. Qu'est-ce que FDg ?</h2>
<p>
<b>FDg</b> est une interface graphique moderne, rapide et intuitive pour l'utilitaire de recherche Linux ultra-rapide <code>fd</code> (nommé <code>fdfind</code> sur Debian et Ubuntu).
FDg vous permet de trouver instantanément des fichiers et dossiers partout sur votre disque, avec prise en charge complète des caractères génériques style MS-DOS (<code>*</code> et <code>?</code>), tri naturel et intégration parfaite au bureau.
</p>

<h2>2. Comment effectuer une recherche</h2>
<ol>
    <li><b>Champ Rechercher :</b> Entrez le nom de fichier ou le motif recherché (ex. <code>document*.pdf</code> ou <code>*rapport*</code>).</li>
    <li><b>Dossier de départ :</b>
        <ul>
            <li>Défini par défaut sur la racine du disque <code>/</code>.</li>
            <li>Cliquez sur <b>Home</b> pour définir instantanément votre dossier utilisateur personnel.</li>
            <li>Cliquez sur <b>Parcourir...</b> pour sélectionner un dossier personnalisé.</li>
        </ul>
    </li>
    <li><b>Filtres et options :</b>
        <ul>
            <li><b>Fichiers cachés (-H) :</b> Recherche également parmi les éléments cachés.</li>
            <li><b>Sensible à la casse (-s) :</b> Recherche sensible aux majuscules/minuscules.</li>
            <li><b>Suivre les liens symboliques (-L) :</b> Parcourt les liens symboliques.</li>
            <li><b>Caractères génériques (* et ?) :</b> Activé par défaut pour les motifs DOS.</li>
            <li><b>Type d'élément :</b> <i>Tous les éléments</i>, <i>Fichiers uniquement</i> ou <i>Dossiers uniquement</i>.</li>
        </ul>
    </li>
    <li>Cliquez sur <b>Rechercher</b> ou appuyez sur la touche <b>Entrée</b>.</li>
</ol>

<h2>3. Caractères génériques style MS-DOS (* et ?)</h2>
<table>
    <tr><th>Exemple</th><th>Description</th></tr>
    <tr><td><code>*.txt</code></td><td>Tous les fichiers se terminant par .txt</td></tr>
    <tr><td><code>*.*</code></td><td>Tous les fichiers possédant une extension</td></tr>
    <tr><td><code>nom*.jpg</code></td><td>Fichiers commençant par 'nom' et se terminant par '.jpg'</td></tr>
    <tr><td><code>doc*</code></td><td>Fichiers et dossiers commençant par 'doc'</td></tr>
    <tr><td><code>*rapport*</code></td><td>Fichiers contenant le mot 'rapport' dans le nom</td></tr>
    <tr><td><code>photo?.png</code></td><td>Remplace un seul caractère (ex. photo1.png, photoA.png)</td></tr>
    <tr><td><code>*</code></td><td>Affiche tous les éléments du dossier sélectionné</td></tr>
</table>

<h2>4. Annulation Immédiate et Arrêt Forcé du Processus (Kill App)</h2>
<div class="callout callout-success">
<b>Sécurité et Performances Garanties :</b><br/>
Lorsque vous cliquez sur <b>Interrompre</b>, le processus d'arrière-plan <code>fd</code> est immédiatement arrêté via <code>SIGKILL</code>.<br/>
<b>Pourquoi est-ce 100% sûr ?</b> L'outil <code>fd</code> effectue uniquement des opérations de lecture (read-only) sur les métadonnées du système de fichiers. Il n'écrit, ne modifie et ne supprime jamais aucun fichier. L'arrêt forcé est donc totalement sûr pour votre système d'exploitation et vos documents, tout en libérant instantanément le processeur (CPU) et la mémoire vive (RAM).
</div>

<h2>5. Résultats et Actions</h2>
<ul>
    <li><b>Double-clic sur Nom de fichier :</b> Ouvre directement le fichier avec l'application par défaut de votre système.</li>
    <li><b>Double-clic sur Chemin :</b> Ouvre le gestionnaire de fichiers (Dolphin, Nautilus, etc.) et <b>met en surbrillance l'élément sélectionné sans l'ouvrir</b>.</li>
    <li><b>Clic sur l'en-tête de colonne :</b> Trie la liste par ordre alphabétique croissant ou décroissant.</li>
    <li><b>Clic droit (Menu contextuel) :</b> Options pour ouvrir le fichier, afficher dans le dossier, copier le chemin ou copier le nom.</li>
</ul>

<div class="footer">
FDg v1.2.0 | Idée : PsyTech76 | Développé avec l'IA | Logiciel Gratuit Open Source
</div>
</body>
</html>
"""

files = {
    "guide_it.html": it_html,
    "guide_en.html": en_html,
    "guide_de.html": de_html,
    "guide_es.html": es_html,
    "guide_fr.html": fr_html,
}

for fn, content in files.items():
    p = os.path.join(help_dir, fn)
    with open(p, "w", encoding="utf-8") as f:
        f.write(content.strip())
    print(f"Creato {p} ({os.path.getsize(p)} bytes)")
