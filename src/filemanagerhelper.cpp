#include "filemanagerhelper.h"

#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDebug>

QProcessEnvironment FileManagerHelper::cleanEnvironment()
{
    // Partiamo dall'ambiente di sistema corrente
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // In un'AppImage, AppRun esporta APPIMAGE_ORIGINAL_LD_LIBRARY_PATH prima di manipolare LD_LIBRARY_PATH.
    // Se presente, ripristiniamo il valore originale; altrimenti rimuoviamo completamente la variabile
    // in modo che il caricatore dinamico (ld.so) usi esclusivamente i percorsi standard di sistema.
    if (env.contains(QStringLiteral("APPIMAGE_ORIGINAL_LD_LIBRARY_PATH"))) {
        QString origLd = env.value(QStringLiteral("APPIMAGE_ORIGINAL_LD_LIBRARY_PATH"));
        if (!origLd.isEmpty()) {
            env.insert(QStringLiteral("LD_LIBRARY_PATH"), origLd);
        } else {
            env.remove(QStringLiteral("LD_LIBRARY_PATH"));
        }
    } else {
        env.remove(QStringLiteral("LD_LIBRARY_PATH"));
    }

    // Ripristiniamo QT_PLUGIN_PATH per evitare che applicazioni Qt di sistema (come Kate, VLC o Dolphin)
    // tentino di caricare i plugin Qt6 inclusi nel pacchetto AppImage causando crash istantanei.
    if (env.contains(QStringLiteral("APPIMAGE_ORIGINAL_QT_PLUGIN_PATH"))) {
        QString origQt = env.value(QStringLiteral("APPIMAGE_ORIGINAL_QT_PLUGIN_PATH"));
        if (!origQt.isEmpty()) {
            env.insert(QStringLiteral("QT_PLUGIN_PATH"), origQt);
        } else {
            env.remove(QStringLiteral("QT_PLUGIN_PATH"));
        }
    } else {
        env.remove(QStringLiteral("QT_PLUGIN_PATH"));
    }

    // Rimuoviamo altre variabili Qt o QML potenzialmente iniettate dal bundle AppImage
    env.remove(QStringLiteral("QML2_IMPORT_PATH"));
    env.remove(QStringLiteral("QT_QPA_PLATFORMTHEME"));
    env.remove(QStringLiteral("QT_QPA_PLATFORM"));

    return env;
}

bool FileManagerHelper::showItemInFolder(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        qWarning() << "[FileManagerHelper] File non esistente per showItemInFolder:" << filePath;
        return false;
    }

    const QString absolutePath = fileInfo.absoluteFilePath();
    const QString parentDirPath = fileInfo.absolutePath();

    // -------------------------------------------------------------------------
    // METODO 1: FreeDesktop DBus org.freedesktop.FileManager1.ShowItems
    // Standard moderno Linux supportato da GNOME (Nautilus), KDE (Dolphin),
    // Cinnamon (Nemo) e XFCE (Thunar).
    // Essendo una chiamata IPC su socket DBus, non soffre di contaminazioni di librerie.
    // -------------------------------------------------------------------------
    QDBusInterface fileManagerInterface(
        QStringLiteral("org.freedesktop.FileManager1"),
        QStringLiteral("/org/freedesktop/FileManager1"),
        QStringLiteral("org.freedesktop.FileManager1"),
        QDBusConnection::sessionBus()
    );

    if (fileManagerInterface.isValid()) {
        QStringList uris;
        uris << QUrl::fromLocalFile(absolutePath).toString();

        QDBusMessage reply = fileManagerInterface.call(QStringLiteral("ShowItems"), uris, QString());
        if (reply.type() != QDBusMessage::ErrorMessage) {
            return true;
        }
        qDebug() << "[FileManagerHelper] DBus ShowItems non disponibile o fallito:" << reply.errorMessage();
    }

    // -------------------------------------------------------------------------
    // METODO 2: Esecuzione diretta del file manager con ambiente isolato e pulito
    // -------------------------------------------------------------------------
    QProcessEnvironment env = cleanEnvironment();

    // Nautilus (GNOME)
    {
        QProcess proc;
        proc.setProcessEnvironment(env);
        proc.setProgram(QStringLiteral("nautilus"));
        proc.setArguments(QStringList() << QStringLiteral("--select") << absolutePath);
        if (proc.startDetached()) {
            return true;
        }
    }

    // Dolphin (KDE Plasma)
    {
        QProcess proc;
        proc.setProcessEnvironment(env);
        proc.setProgram(QStringLiteral("dolphin"));
        proc.setArguments(QStringList() << QStringLiteral("--select") << absolutePath);
        if (proc.startDetached()) {
            return true;
        }
    }

    // Nemo (Cinnamon / Linux Mint)
    {
        QProcess proc;
        proc.setProcessEnvironment(env);
        proc.setProgram(QStringLiteral("nemo"));
        proc.setArguments(QStringList() << QStringLiteral("--no-desktop") << absolutePath);
        if (proc.startDetached()) {
            return true;
        }
    }

    // Thunar (XFCE / Debian XFCE)
    {
        QProcess proc;
        proc.setProcessEnvironment(env);
        proc.setProgram(QStringLiteral("thunar"));
        proc.setArguments(QStringList() << absolutePath);
        if (proc.startDetached()) {
            return true;
        }
    }

    // PCManFM / PCManFM-Qt (LXDE / LXQt / Debian)
    {
        QProcess proc;
        proc.setProcessEnvironment(env);
        proc.setProgram(QStringLiteral("pcmanfm"));
        proc.setArguments(QStringList() << parentDirPath);
        if (proc.startDetached()) {
            return true;
        }
    }

    // -------------------------------------------------------------------------
    // METODO 3: Fallback apertura cartella contenitrice
    // -------------------------------------------------------------------------
    qDebug() << "[FileManagerHelper] Fallback: apertura cartella genitore:" << parentDirPath;
    return openFile(parentDirPath);
}

bool FileManagerHelper::openFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        qWarning() << "[FileManagerHelper] Impossibile aprire file inesistente:" << filePath;
        return false;
    }

    const QString absPath = fileInfo.absoluteFilePath();
    QProcessEnvironment env = cleanEnvironment();

    // -------------------------------------------------------------------------
    // METODO 1: Avvio diretto di xdg-open con ambiente pulito
    // In questo modo l'applicazione associata (es. Kate, VLC, Gedit, LibreOffice)
    // non eredita le librerie Qt interne all'AppImage ed esegue regolarmente.
    // -------------------------------------------------------------------------
    {
        QProcess proc;
        proc.setProcessEnvironment(env);
        proc.setProgram(QStringLiteral("xdg-open"));
        proc.setArguments(QStringList() << absPath);
        if (proc.startDetached()) {
            qDebug() << "[FileManagerHelper] File aperto con successo via xdg-open:" << absPath;
            return true;
        }
    }

    // -------------------------------------------------------------------------
    // METODO 2: Fallback con gio open (GNOME / GLib desktop utility)
    // -------------------------------------------------------------------------
    {
        QProcess proc;
        proc.setProcessEnvironment(env);
        proc.setProgram(QStringLiteral("gio"));
        proc.setArguments(QStringList() << QStringLiteral("open") << absPath);
        if (proc.startDetached()) {
            qDebug() << "[FileManagerHelper] File aperto con successo via gio open:" << absPath;
            return true;
        }
    }

    // -------------------------------------------------------------------------
    // METODO 3: Fallback finale con QDesktopServices standard
    // -------------------------------------------------------------------------
    qDebug() << "[FileManagerHelper] Tentativo finale con QDesktopServices:" << absPath;
    return QDesktopServices::openUrl(QUrl::fromLocalFile(absPath));
}
