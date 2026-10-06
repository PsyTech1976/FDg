#ifndef DEPENDENCYINSTALLER_H
#define DEPENDENCYINSTALLER_H

#include <QDialog>
#include <QProcess>

class QLabel;
class QProgressBar;
class QPlainTextEdit;
class QPushButton;

/**
 * @file dependencyinstaller.h
 * @brief Finestra di dialogo per l'installazione automatica guidata di 'fd' / 'fd-find'.
 *
 * NOTE ARCHITETTURALI PER FUTURE REVISIONI (UMANO O IA):
 * ==============================================================================
 * 1. RILEVAMENTO AUTOMATICO DEL GESTORE PACCHETTI:
 *    - Arch Linux e derivate (CachyOS, Manjaro, EndeavourOS): usa 'pacman' (pacchetto 'fd')
 *    - Debian, Ubuntu e derivate (Mint, Pop!_OS): usa 'apt-get' (pacchetto 'fd-find')
 *    - Fedora e RHEL: usa 'dnf' (pacchetto 'fd-find')
 *    - openSUSE: usa 'zypper' (pacchetto 'fd')
 *
 * 2. ELEVAZIONE PRIVILEGI GRAFICA (GUI):
 *    - Utilizza 'pkexec' (PolicyKit), delegando in sicurezza l'autorizzazione alla
 *      finestra nativa del desktop environment (KDE Plasma, GNOME, XFCE).
 *
 * 3. ISOLAMENTO RUNTIME APPIMAGE:
 *    - Le chiamate ai gestori di pacchetti e comandi di sistema usano un ambiente
 *      epurato (FileManagerHelper::cleanEnvironment()) per evitare che le librerie
 *      interne dell'AppImage collidano con quelle di sistema.
 *
 * 4. LOGGING IN TEMPO REALE E FEEDBACK:
 *    - Lo standard output e lo standard error vengono catturati in tempo reale tramite
 *      readyReadStandardOutput() / readyReadStandardError() e visualizzati in una
 *      vista a scorrimento automatico, mostrando le percentuali di download e pacchetti.
 * ==============================================================================
 */
class DependencyInstallerDialog : public QDialog
{
    Q_OBJECT

public:
    enum class PackageManagerType {
        Pacman,
        Apt,
        Dnf,
        Zypper,
        Unknown
    };

    explicit DependencyInstallerDialog(QWidget *parent = nullptr);
    ~DependencyInstallerDialog() override;

    static PackageManagerType detectPackageManager();
    static QString packageManagerName(PackageManagerType type);
    static QString requiredPackageName(PackageManagerType type);

    bool wasSuccessful() const { return m_installationSuccess; }

public slots:
    void startInstallation();

private slots:
    void onProcessReadyReadStandardOutput();
    void onProcessReadyReadStandardError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessErrorOccurred(QProcess::ProcessError error);

private:
    void setupUi();
    void appendLog(const QString &text, bool isError = false);

    QLabel *m_headerLabel;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QPlainTextEdit *m_logEdit;
    QPushButton *m_installButton;
    QPushButton *m_closeButton;

    QProcess *m_process;
    PackageManagerType m_pkgType;
    bool m_installationSuccess;
};

#endif // DEPENDENCYINSTALLER_H
