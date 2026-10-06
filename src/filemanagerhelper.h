#ifndef FILEMANAGERHELPER_H
#define FILEMANAGERHELPER_H

#include <QString>
#include <QProcessEnvironment>

/**
 * @brief Classe di supporto per l'interazione con il gestore file di sistema (File Manager).
 *
 * Fornisce metodi statici per interagire con l'ambiente desktop Linux, consentendo di:
 * 1. Aprire un file (testo, audio, video, immagini, pdf, ecc.) con l'applicazione di sistema predefinita.
 * 2. Aprire la cartella contenitrice evidenziando/selezionando il file specifico senza eseguirlo.
 *
 * NOTA SULL'AMBIENTE APPIMAGE:
 * Quando l'applicazione viene eseguita come AppImage, le variabili d'ambiente LD_LIBRARY_PATH e
 * QT_PLUGIN_PATH contengono le librerie interne del pacchetto. Se un'applicazione esterna
 * (es. Kate, VLC, Gedit, Dolphin) viene avviata ereditando tali variabili, fallisce o va in crash
 * a causa di conflitti tra i plugin/librerie Qt dell'AppImage e quelli di sistema.
 * Questa classe pulisce opportunamente l'ambiente prima di invocare comandi esterni.
 */
class FileManagerHelper
{
public:
    /**
     * @brief Restituisce un ambiente di processo pulito privo delle variabili interne dell'AppImage.
     * @return QProcessEnvironment configurato per lanciare processi esterni di sistema in sicurezza.
     */
    static QProcessEnvironment cleanEnvironment();

    /**
     * @brief Apre la cartella contenente il file ed evidenzia/seleziona il file.
     *
     * @param filePath Percorso assoluto del file da mostrare.
     * @return true se l'operazione ha avuto successo, altrimenti false.
     */
    static bool showItemInFolder(const QString &filePath);

    /**
     * @brief Apre il file specificato utilizzando l'applicazione predefinita di sistema.
     *
     * Utilizza xdg-open (o gio open) avviato in un ambiente di processo isolato e pulito.
     *
     * @param filePath Percorso assoluto del file da aprire.
     * @return true se il file è stato aperto correttamente, altrimenti false.
     */
    static bool openFile(const QString &filePath);
};

#endif // FILEMANAGERHELPER_H
