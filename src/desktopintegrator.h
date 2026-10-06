#ifndef DESKTOPINTEGRATOR_H
#define DESKTOPINTEGRATOR_H

#include <QString>

/**
 * @file desktopintegrator.h
 * @brief Gestore dell'integrazione desktop XDG (Menu applicazioni e Barra delle applicazioni) per FDg.
 *
 * NOTE ARCHITETTURALI PER FUTURE REVISIONI (UMANO O IA):
 * ==============================================================================
 * 1. IL PROBLEMA DELLE APPIMAGE SUL DESKTOP LINUX (KDE / GNOME / XFCE):
 *    Un'applicazione in formato AppImage viene eseguita montando un filesystem temporaneo
 *    in '/tmp/.mount_XXXXXX/'. Quando l'utente aggiunge l'icona ai preferiti o la blocca
 *    alla barra delle applicazioni (Task Manager), il desktop environment punta ai file
 *    temporanei (.desktop e icona) all'interno di '/tmp/.mount_XXXXXX/'.
 *    Non appena l'AppImage viene chiusa, la cartella temporanea viene smontata e distrutta:
 *    - L'icona diventa invisibile / vuota (perché il file grafico è sparito).
 *    - Il programma non si avvia più dalla barra (perché il percorso è inesistente).
 *
 * 2. LA SOLUZIONE IMPLEMENTATA DA QUESTA CLASSE:
 *    - Crea un lanciatore XDG permanente in '~/.local/share/applications/fdg.desktop'.
 *    - Copia l'icona dell'applicazione in '~/.local/share/icons/fdg.png'.
 *    - Rileva automaticamente la posizione reale e permanente dell'AppImage tramite
 *      la variabile d'ambiente 'APPIMAGE' impostata dal runtime di AppImage (oppure
 *      QCoreApplication::applicationFilePath() se eseguito come binario normale).
 *    - Imposta 'StartupWMClass=fdg' per agganciare in modo impeccabile la finestra Qt
 *      in esecuzione al lanciatore registrato (compatibile sia con X11 sia con Wayland).
 *    - Notifica il sistema con 'update-desktop-database' per aggiornare subito i menu.
 *    - Permette la rimozione pulita e completa (disinstallazione del lanciatore) su richiesta.
 * ==============================================================================
 */
class DesktopIntegrator
{
public:
    /**
     * @brief Verifica se FDg è attualmente integrato nel sistema (se esiste il file .desktop).
     * @return true se il lanciatore ~/.local/share/applications/fdg.desktop è presente.
     */
    static bool isIntegrated();

    /**
     * @brief Installa il lanciatore desktop permanente e l'icona nel profilo utente.
     * @param errorMessage In caso di errore, conterrà la descrizione del problema.
     * @return true se l'integrazione è avvenuta con successo.
     */
    static bool installIntegration(QString &errorMessage);

    /**
     * @brief Rimuove il lanciatore desktop e l'icona dal profilo utente.
     * @param errorMessage In caso di errore, conterrà la descrizione del problema.
     * @return true se la rimozione è avvenuta con successo.
     */
    static bool removeIntegration(QString &errorMessage);

    /**
     * @brief Restituisce il percorso assoluto del file .desktop target.
     */
    static QString desktopFilePath();

    /**
     * @brief Restituisce il percorso assoluto dell'icona target.
     */
    static QString iconFilePath();

    /**
     * @brief Determina il percorso effettivo e permanente dell'eseguibile/AppImage.
     */
    static QString currentExecutablePath();
};

#endif // DESKTOPINTEGRATOR_H
