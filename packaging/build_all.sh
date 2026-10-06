#!/bin/bash
# ==============================================================================
# Script di compilazione universale per FDg (Compatibile Arch e Debian)
# ==============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== [1/3] Compilazione ed estrazione librerie con Debian 12 (glibc 2.36) ==="
docker run --rm -v "${PROJECT_DIR}:/workspace" fd-frontend-builder:debian12 sh -c "
    set -e
    mkdir -p /workspace/build-debian
    cd /workspace/build-debian
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j\$(nproc)
    mkdir -p /workspace/build
    cp /workspace/build-debian/fdg /workspace/build/fdg
    python3 /workspace/packaging/bundle_appimage.py --appdir-only
"

echo "=== [2/3] Generazione AppImage universale ==="
ARCH=x86_64 appimagetool -n "${PROJECT_DIR}/AppDir" "${PROJECT_DIR}/FDg-x86_64.AppImage"

echo "=== [3/3] Verifica compatibilità binario ==="
objdump -p "${PROJECT_DIR}/build/fdg" | grep -A 8 "required from libc.so.6"

echo "=== Costruzione completata con successo! ==="

