#include <QApplication>
#include <QIcon>
#include "mainwindow.h"

/**
 * @file main.cpp
 * @brief Punto di ingresso principale dell'applicazione FDg.
 *
 * GUIDA E NOTE ARCHITETTURALI PER FUTURE REVISIONI (UMANO O IA):
 * ==============================================================================
 * 1. SCOPO E ARCHITETTURA DEL PROGETTO:
 *    FDg è un'interfaccia grafica moderna (Qt 6 Widgets, C++20) progettata per
 *    fungere da frontend ad altissime prestazioni per l'utility 'fd' ('fdfind').
 *    L'interfaccia grafica è rigorosamente separata dalla logica applicativa
 *    (file mainwindow.ui modificabile con Qt Designer).
 *
 * 2. COMPATIBILITÀ CROSS-DISTRIBUTION (DEBIAN 12+ & ARCH LINUX):
 *    - Rilevamento automatico eseguibile: 'fd' su Arch/Fedora, 'fdfind' su Debian/Ubuntu.
 *    - Portabilità AppImage: compilata all'interno di un container Debian 12 (Bookworm)
 *      con requisiti GLIBC 2.34/2.36 retrocompatibili con tutti i moderni sistemi Linux.
 *    - Isolamento runtime: le chiamate alle applicazioni di sistema (FileManagerHelper)
 *      utilizzano un ambiente purificato per non ereditare LD_LIBRARY_PATH dell'AppImage.
 *
 * 3. SICUREZZA E KILL DEL PROCESSO IN BACKGROUND:
 *    - L'annullamento della ricerca esegue un SIGKILL immediato al processo 'fd' e al
 *      relativo process group (setpgid).
 *    - Sicurezza: 'fd' opera in sola lettura (read-only), rendendo il kill forzato
 *      totalmente sicuro per il filesystem e i documenti, rilasciando subito CPU e RAM.
 *
 * 4. INTERNAZIONALIZZAZIONE (i18n) E GUIDA INTEGRATA:
 *    - 5 lingue supportate: Italiano, Inglese, Tedesco, Spagnolo, Francese.
 *    - Bandiere grafiche (SVG/PNG) indipendenti da font emoji di sistema.
 *    - Guida HTML incorporata nelle risorse binarie (resources.qrc) visualizzata
 *      tramite browser minimale nativo (GuideDialog / QTextBrowser).
 *
 * Idea originale: PsyTech76
 * Realizzazione: Assistente IA
 * Licenza: Gratuita / Open Source
 * ==============================================================================
 */
int main(int argc, char *argv[])
{
    // Inizializza l'applicazione Qt Widgets
    QApplication app(argc, argv);

    // Metadati dell'applicazione per integrazione desktop (X11 / Wayland)
    QApplication::setApplicationName(QStringLiteral("FDg"));
    QApplication::setApplicationDisplayName(QStringLiteral("FDg"));
    QApplication::setOrganizationName(QStringLiteral("PsyTech76"));
    QApplication::setApplicationVersion(QStringLiteral("1.2.0"));
    QApplication::setDesktopFileName(QStringLiteral("fdg"));

    // Icona globale dell'applicazione
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/appicon.png")));

    // Creazione e visualizzazione della finestra principale
    MainWindow window;
    window.show();

    return app.exec();
}
