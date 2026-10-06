#!/usr/bin/env python3
# ==============================================================================
# BUNDLE APPIMAGE CREATOR PER FDg
# ==============================================================================
# Questo script automatizza l'assemblaggio della struttura AppDir e la creazione
# del pacchetto universale FDg-x86_64.AppImage.
#
# NOTE ARCHITETTURALI PER FUTURE REVISIONI (UMANO O IA):
# 1. Compatibilità Glibc (Debian 12 & Arch Linux):
#    L'eseguibile 'fdg' viene compilato all'interno di un container Debian 12
#    (glibc 2.36). Le librerie di sistema fondamentali (libc, libm, ld-linux, ecc.)
#    vengono ESCLUSE deliberatamente dal bundle tramite SYSTEM_EXCLUDES in modo
#    che vengano caricate direttamente dal sistema host dell'utente, prevenendo
#    crash da collisione di simboli (symbol lookup error / version GLIBC not found).
# 2. Plugin Qt6:
#    Vengono incorporati i plugin essenziali (platforms/libqxcb.so, imageformats,
#    iconengines/libqsvgicon.so) e relative dipendenze dinamiche.
# 3. AppRun:
#    Viene generato uno script AppRun che configura LD_LIBRARY_PATH e QT_PLUGIN_PATH
#    per il processo interno senza inquinare l'ambiente desktop.
# ==============================================================================
import os
import sys
import shutil
import subprocess
import re

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
APPDIR = os.path.join(PROJECT_DIR, "AppDir")
OUTPUT_APPIMAGE = os.path.join(PROJECT_DIR, "FDg-x86_64.AppImage")

# Elenco librerie di sistema da escludere dal bundle per evitare incompatibilità tra distribuzioni
SYSTEM_EXCLUDES = {
    "ld-linux-x86-64.so.2",
    "libc.so.6",
    "libm.so.6",
    "libdl.so.2",
    "libpthread.so.0",
    "librt.so.1",
    "libresolv.so.2",
    "libgcc_s.so.1",
    "libdrm.so.2",
    "libasound.so.2",
}

def run(cmd, cwd=None):
    print(f"[RUN] {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    res = subprocess.run(cmd, shell=isinstance(cmd, str), cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res.returncode != 0:
        print(f"Error executing command: {res.stderr}")
        sys.exit(res.returncode)
    return res.stdout

def get_dependencies(elf_path):
    deps = set()
    try:
        out = subprocess.check_output(["ldd", elf_path], text=True, stderr=subprocess.DEVNULL)
        for line in out.splitlines():
            line = line.strip()
            # Match pattern: libname.so => /path/to/lib (0x...)
            m = re.search(r'([^\s]+)\s*=>\s*([^\s]+)', line)
            if m:
                lib_name = m.group(1)
                lib_path = m.group(2)
                if os.path.isfile(lib_path):
                    deps.add((lib_name, lib_path))
            else:
                # Direct match: /lib64/ld-linux-x86-64.so.2 (0x...)
                m2 = re.match(r'(/[^\s]+)', line)
                if m2:
                    p = m2.group(1)
                    deps.add((os.path.basename(p), p))
    except Exception as e:
        print(f"ldd error on {elf_path}: {e}")
    return deps

def main():
    print("=== Costruzione dell'AppImage per FDg ===")

    # 1. Ricostruzione dell'AppDir
    if os.path.exists(APPDIR):
        shutil.rmtree(APPDIR)
    os.makedirs(os.path.join(APPDIR, "usr", "bin"), exist_ok=True)
    os.makedirs(os.path.join(APPDIR, "usr", "lib"), exist_ok=True)
    os.makedirs(os.path.join(APPDIR, "usr", "plugins", "platforms"), exist_ok=True)
    os.makedirs(os.path.join(APPDIR, "usr", "plugins", "imageformats"), exist_ok=True)
    os.makedirs(os.path.join(APPDIR, "usr", "plugins", "iconengines"), exist_ok=True)
    os.makedirs(os.path.join(APPDIR, "usr", "plugins", "styles"), exist_ok=True)
    os.makedirs(os.path.join(APPDIR, "usr", "share", "applications"), exist_ok=True)
    os.makedirs(os.path.join(APPDIR, "usr", "share", "icons", "hicolor", "scalable", "apps"), exist_ok=True)
    os.makedirs(os.path.join(APPDIR, "usr", "share", "icons", "hicolor", "256x256", "apps"), exist_ok=True)

    # 2. Copia eseguibile principale (cerca sia 'fdg' che 'fd-frontend')
    bin_candidates = [
        os.path.join(PROJECT_DIR, "build", "fdg"),
        os.path.join(PROJECT_DIR, "build-debian", "fdg"),
        os.path.join(PROJECT_DIR, "build", "fd-frontend"),
        os.path.join(PROJECT_DIR, "build-debian", "fd-frontend")
    ]
    bin_src = None
    for cand in bin_candidates:
        if os.path.exists(cand):
            bin_src = cand
            break

    if not bin_src:
        print(f"Errore: binario non trovato in {bin_candidates}. Compilalo prima con cmake.")
        sys.exit(1)

    print(f"Utilizzo binario sorgente: {bin_src}")
    bin_dst = os.path.join(APPDIR, "usr", "bin", "fdg")
    shutil.copy2(bin_src, bin_dst)
    os.chmod(bin_dst, 0o755)

    # Crea anche symlink fd-frontend per compatibilità retroattiva
    symlink_compat = os.path.join(APPDIR, "usr", "bin", "fd-frontend")
    if os.path.exists(symlink_compat):
        os.remove(symlink_compat)
    os.symlink("fdg", symlink_compat)

    # 3. Ricerca dinamica della directory dei plugin Qt6 (supporto Debian, Ubuntu, Arch, Fedora)
    possible_plugin_dirs = [
        "/usr/lib/qt6/plugins",
        "/usr/lib/x86_64-linux-gnu/qt6/plugins",
        "/usr/lib64/qt6/plugins"
    ]
    try:
        qmake_out = subprocess.check_output(["qmake6", "-query", "QT_INSTALL_PLUGINS"], text=True).strip()
        if qmake_out and os.path.isdir(qmake_out):
            possible_plugin_dirs.insert(0, qmake_out)
    except Exception:
        pass

    qt_plugins_dir = None
    for p_dir in possible_plugin_dirs:
        if os.path.isdir(os.path.join(p_dir, "platforms")):
            qt_plugins_dir = p_dir
            break

    if not qt_plugins_dir:
        print("ATTENZIONE: Cartella plugin Qt6 non trovata, uso fallback:", possible_plugin_dirs[0])
        qt_plugins_dir = possible_plugin_dirs[0]
    else:
        print("Cartella plugin Qt6 rilevata:", qt_plugins_dir)

    platform_plugins = ["libqxcb.so", "libqwayland.so", "libqoffscreen.so"]
    for p in platform_plugins:
        src = os.path.join(qt_plugins_dir, "platforms", p)
        if os.path.exists(src):
            shutil.copy2(src, os.path.join(APPDIR, "usr", "plugins", "platforms", p))

    # Imageformats (SVG, PNG, ICO, JPEG)
    for p in ["libqsvg.so", "libqjpeg.so", "libqico.so"]:
        src = os.path.join(qt_plugins_dir, "imageformats", p)
        if os.path.exists(src):
            shutil.copy2(src, os.path.join(APPDIR, "usr", "plugins", "imageformats", p))

    # Iconengines (SVG icon engine)
    for p in ["libqsvgicon.so"]:
        src = os.path.join(qt_plugins_dir, "iconengines", p)
        if os.path.exists(src):
            shutil.copy2(src, os.path.join(APPDIR, "usr", "plugins", "iconengines", p))

    # 4. Desktop file & Icone
    desktop_src = os.path.join(PROJECT_DIR, "packaging", "fdg.desktop")
    shutil.copy2(desktop_src, os.path.join(APPDIR, "fdg.desktop"))
    shutil.copy2(desktop_src, os.path.join(APPDIR, "usr", "share", "applications", "fdg.desktop"))

    icon_png = os.path.join(PROJECT_DIR, "resources", "icons", "appicon_256.png")
    icon_svg = os.path.join(PROJECT_DIR, "resources", "icons", "appicon.svg")
    shutil.copy2(icon_png, os.path.join(APPDIR, "appicon.png"))
    shutil.copy2(icon_png, os.path.join(APPDIR, ".DirIcon"))
    shutil.copy2(icon_png, os.path.join(APPDIR, "usr", "share", "icons", "hicolor", "256x256", "apps", "appicon.png"))
    shutil.copy2(icon_svg, os.path.join(APPDIR, "usr", "share", "icons", "hicolor", "scalable", "apps", "appicon.svg"))

    # 5. Risoluzione e copia dipendenze librerie (.so)
    all_binaries = [bin_dst]
    for root, _, files in os.walk(os.path.join(APPDIR, "usr", "plugins")):
        for f in files:
            if f.endswith(".so"):
                all_binaries.append(os.path.join(root, f))

    collected_libs = set()
    to_inspect = list(all_binaries)
    visited_files = set()

    while to_inspect:
        current = to_inspect.pop()
        if current in visited_files:
            continue
        visited_files.add(current)

        deps = get_dependencies(current)
        for lib_name, lib_path in deps:
            if lib_name in SYSTEM_EXCLUDES or any(lib_name.startswith(p) for p in ["libc.so", "ld-linux", "libm.so", "libdl.so", "libpthread.so"]):
                continue

            target_path = os.path.join(APPDIR, "usr", "lib", lib_name)
            if not os.path.exists(target_path):
                try:
                    shutil.copy2(lib_path, target_path)
                    to_inspect.append(target_path)
                except Exception as e:
                    print(f"Impossibile copiare {lib_path}: {e}")

    # 6. Creazione script AppRun con salvataggio delle variabili ambientali originali
    apprun_content = """#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
# Salva le variabili di sistema per permettere alle applicazioni esterne (es. Kate, VLC, Gedit)
# lanciate tramite xdg-open di ripristinare il corretto ambiente di sistema ed evitare conflitti con Qt6
export APPIMAGE_ORIGINAL_LD_LIBRARY_PATH="${LD_LIBRARY_PATH}"
export APPIMAGE_ORIGINAL_QT_PLUGIN_PATH="${QT_PLUGIN_PATH}"
export APPIMAGE_ORIGINAL_XDG_DATA_DIRS="${XDG_DATA_DIRS}"

export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${HERE}/usr/plugins"
export XDG_DATA_DIRS="${HERE}/usr/share:${XDG_DATA_DIRS}"
exec "${HERE}/usr/bin/fdg" "$@"
"""
    apprun_path = os.path.join(APPDIR, "AppRun")
    with open(apprun_path, "w", encoding="utf-8") as f:
        f.write(apprun_content)
    os.chmod(apprun_path, 0o755)

    # 7. Esecuzione di appimagetool (se non richiesta la sola preparazione di AppDir)
    if "--appdir-only" in sys.argv:
        print(f"=== AppDir preparata con successo in: {APPDIR} ===")
        return

    if os.path.exists(OUTPUT_APPIMAGE):
        os.remove(OUTPUT_APPIMAGE)

    env = os.environ.copy()
    env["ARCH"] = "x86_64"
    print("Creazione pacchetto AppImage con appimagetool...")
    subprocess.run(["appimagetool", "-n", APPDIR, OUTPUT_APPIMAGE], env=env, check=True)
    os.chmod(OUTPUT_APPIMAGE, 0o755)

    # Crea anche symlink o copia FD-FrontEnd per retrocompatibilità
    compat_appimage = os.path.join(PROJECT_DIR, "FD-FrontEnd-x86_64.AppImage")
    if os.path.exists(compat_appimage):
        os.remove(compat_appimage)
    shutil.copy2(OUTPUT_APPIMAGE, compat_appimage)

    print(f"=== AppImage completata con successo: {OUTPUT_APPIMAGE} ===")

if __name__ == "__main__":
    main()
