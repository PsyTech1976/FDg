#include "desktopintegrator.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProcess>
#include <QTextStream>
#include <QDebug>

QString DesktopIntegrator::desktopFilePath()
{
    QString appsDir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    if (appsDir.isEmpty()) {
        appsDir = QDir::homePath() + QStringLiteral("/.local/share/applications");
    }
    return appsDir + QStringLiteral("/fdg.desktop");
}

QString DesktopIntegrator::iconFilePath()
{
    return QDir::homePath() + QStringLiteral("/.local/share/icons/fdg.png");
}

QString DesktopIntegrator::currentExecutablePath()
{
    // Rileva la variabile d'ambiente impostata dai runtime AppImage
    QString appImagePath = qEnvironmentVariable("APPIMAGE");
    if (!appImagePath.isEmpty() && QFile::exists(appImagePath)) {
        return appImagePath;
    }
    return QCoreApplication::applicationFilePath();
}

bool DesktopIntegrator::isIntegrated()
{
    return QFile::exists(desktopFilePath());
}

bool DesktopIntegrator::installIntegration(QString &errorMessage)
{
    // 1. Preparazione della cartella icone e copia del file PNG ad alta risoluzione
    QString iconDst = iconFilePath();
    QString iconDir = QFileInfo(iconDst).absolutePath();
    if (!QDir().mkpath(iconDir)) {
        errorMessage = QStringLiteral("Impossibile creare la cartella icone: ") + iconDir;
        return false;
    }

    QFile iconRes(QStringLiteral(":/icons/appicon_256.png"));
    if (iconRes.open(QIODevice::ReadOnly)) {
        QFile iconOut(iconDst);
        if (iconOut.open(QIODevice::WriteOnly)) {
            iconOut.write(iconRes.readAll());
            iconOut.close();
        } else {
            errorMessage = QStringLiteral("Impossibile salvare l'icona in: ") + iconDst;
            return false;
        }
        iconRes.close();
    } else {
        errorMessage = QStringLiteral("Risorsa icona interna non trovata.");
        return false;
    }

    // 2. Preparazione cartella applicazioni XDG
    QString dPath = desktopFilePath();
    QString appsDir = QFileInfo(dPath).absolutePath();
    if (!QDir().mkpath(appsDir)) {
        errorMessage = QStringLiteral("Impossibile creare la cartella delle applicazioni: ") + appsDir;
        return false;
    }

    // 3. Rilevamento percorso eseguibile
    QString execPath = currentExecutablePath();

    // 4. Scrittura del file .desktop standard
    QFile desktopFile(dPath);
    if (!desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        errorMessage = QStringLiteral("Impossibile creare il file .desktop: ") + desktopFile.errorString();
        return false;
    }

    QTextStream out(&desktopFile);
    out << "[Desktop Entry]\n";
    out << "Type=Application\n";
    out << "Name=FDg\n";
    out << "GenericName=File Search Utility\n";
    out << "GenericName[it]=Strumento di ricerca file\n";
    out << "GenericName[de]=Dateisuchwerkzeug\n";
    out << "GenericName[es]=Herramienta de búsqueda de archivos\n";
    out << "GenericName[fr]=Outil de recherche de fichiers\n";
    out << "Comment=Modern Qt6 GUI Frontend for the fd search command\n";
    out << "Comment[it]=Interfaccia grafica moderna in Qt6 per il comando di ricerca fd (Idea di PsyTech76)\n";
    out << "Comment[de]=Moderne Qt6-GUI für den Suchbefehl fd (Idee von PsyTech76)\n";
    out << "Comment[es]=Interfaz gráfica Qt6 moderna para el comando fd (Idea de PsyTech76)\n";
    out << "Comment[fr]=Interface graphique Qt6 moderne pour la commande fd (Idée de PsyTech76)\n";
    out << "Exec=\"" << execPath << "\" %F\n";
    out << "Icon=" << iconDst << "\n";
    out << "Terminal=false\n";
    out << "Categories=Utility;Filesystem;Qt;System;\n";
    out << "Keywords=fd;fdg;find;search;files;filesystem;ricerca;cerca;\n";
    out << "StartupWMClass=fdg\n";
    out << "StartupNotify=true\n";

    desktopFile.close();

    // Assegna permessi di esecuzione/lettura appropriati
    desktopFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                               QFile::ReadGroup | QFile::ReadOther);

    // 5. Notifica il sistema desktop dell'aggiornamento dei lanciatori
    QProcess::execute(QStringLiteral("update-desktop-database"), QStringList() << appsDir);

    qInfo() << "[DesktopIntegrator] Integrazione desktop installata con successo in:" << dPath;
    return true;
}

bool DesktopIntegrator::removeIntegration(QString &errorMessage)
{
    QString dPath = desktopFilePath();
    if (QFile::exists(dPath)) {
        if (!QFile::remove(dPath)) {
            errorMessage = QStringLiteral("Impossibile eliminare il file: ") + dPath;
            return false;
        }
    }

    QString iPath = iconFilePath();
    if (QFile::exists(iPath)) {
        QFile::remove(iPath);
    }

    QString appsDir = QFileInfo(dPath).absolutePath();
    QProcess::execute(QStringLiteral("update-desktop-database"), QStringList() << appsDir);

    qInfo() << "[DesktopIntegrator] Integrazione desktop rimossa con successo.";
    return true;
}
