#include <QApplication>
#include <QIcon>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QHeaderView>
#include <QStyleFactory>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

#include "mainwindow.h"
#include "guidedialog.h"

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    QApplication::setApplicationName(QStringLiteral("FDg"));
    QApplication::setApplicationDisplayName(QStringLiteral("FDg"));
    QApplication::setOrganizationName(QStringLiteral("PsyTech76"));
    QApplication::setApplicationVersion(QStringLiteral("1.2.0"));
    QApplication::setDesktopFileName(QStringLiteral("fdg"));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/appicon.png")));

    QDir().mkpath(QStringLiteral("docs/images"));

    MainWindow window;
    window.resize(980, 640);
    window.show();
    app.processEvents();

    auto searchEdit = window.findChild<QLineEdit*>(QStringLiteral("searchLineEdit"));
    auto folderEdit = window.findChild<QLineEdit*>(QStringLiteral("folderLineEdit"));
    auto searchBtn  = window.findChild<QPushButton*>(QStringLiteral("searchButton"));
    auto tree       = window.findChild<QTreeWidget*>(QStringLiteral("resultsTreeWidget"));
    auto toggleBtn  = window.findChild<QToolButton*>(QStringLiteral("toggleOptionsButton"));
    auto advWidget  = window.findChild<QWidget*>(QStringLiteral("advancedOptionsWidget"));
    auto langCombo  = window.findChild<QComboBox*>(QStringLiteral("languageComboBox"));
    auto statusLbl  = window.findChild<QLabel*>(QStringLiteral("statusLabel"));
    auto pBar       = window.findChild<QProgressBar*>(QStringLiteral("progressBar"));

    if (!searchEdit || !folderEdit || !tree || !toggleBtn || !advWidget || !langCombo || !statusLbl) {
        qCritical() << "Impossibile trovare uno o piu widget UI!";
        return 1;
    }

    QJsonObject blurMetadata;

    auto addResult = [&](const QString &name, const QString &path) -> SearchResultItem* {
        auto item = new SearchResultItem(name, path, path + "/" + name, tree->topLevelItemCount());
        tree->addTopLevelItem(item);
        return item;
    };

    auto getItemBox = [&](QTreeWidgetItem *item) -> QJsonObject {
        QRect r = tree->visualItemRect(item);
        QPoint topLeft = tree->viewport()->mapTo(&window, r.topLeft());
        QJsonObject box;
        box["x"] = topLeft.x();
        box["y"] = topLeft.y();
        box["width"] = tree->viewport()->width();
        box["height"] = r.height();
        return box;
    };

    // =========================================================================
    // 1. STATO ITALIANO
    // =========================================================================
    langCombo->setCurrentIndex(0); // IT
    app.processEvents();

    searchEdit->setText(QStringLiteral("*.conf"));
    folderEdit->setText(QStringLiteral("/etc"));
    searchBtn->setEnabled(true);
    searchBtn->setText(QStringLiteral("Cerca"));

    tree->clear();
    addResult(QStringLiteral("locale.gen"), QStringLiteral("/etc"));
    addResult(QStringLiteral("resolv.conf"), QStringLiteral("/etc"));
    addResult(QStringLiteral("pacman.conf"), QStringLiteral("/etc"));
    addResult(QStringLiteral("nsswitch.conf"), QStringLiteral("/etc"));
    addResult(QStringLiteral("security.conf"), QStringLiteral("/etc/security"));
    addResult(QStringLiteral("pam.conf"), QStringLiteral("/etc/security"));

    auto sensitiveIt1 = addResult(QStringLiteral("vpn_client_auth.conf"), QStringLiteral("/home/user_private/.vpn/192.168.1.105"));
    auto sensitiveIt2 = addResult(QStringLiteral("db_credentials.conf"), QStringLiteral("/home/user_private/backend/config"));

    addResult(QStringLiteral("sysctl.conf"), QStringLiteral("/etc"));
    addResult(QStringLiteral("fuse.conf"), QStringLiteral("/etc"));
    addResult(QStringLiteral("asound.conf"), QStringLiteral("/etc"));
    addResult(QStringLiteral("makepkg.conf"), QStringLiteral("/etc"));

    pBar->setRange(0, 100);
    pBar->setValue(100);
    statusLbl->setText(QStringLiteral("Completato: 142 elementi in 0.03 s"));

    tree->resizeColumnToContents(0);
    if (tree->columnWidth(0) < 320) tree->setColumnWidth(0, 320);

    // 1.1 Schermata principale compatta (opzioni chiuse)
    advWidget->setVisible(false);
    toggleBtn->setText(QString::fromUtf8("▶ ") + QStringLiteral("Parametri aggiuntivi di ricerca"));
    app.processEvents();
    window.grab().save(QStringLiteral("docs/images/raw_screenshot_main_it.png"));

    QJsonArray itBoxes;
    itBoxes.append(getItemBox(sensitiveIt1));
    itBoxes.append(getItemBox(sensitiveIt2));
    blurMetadata["it_main"] = itBoxes;

    // 1.2 Schermata con opzioni espanse
    advWidget->setVisible(true);
    toggleBtn->setText(QString::fromUtf8("▼ ") + QStringLiteral("Parametri aggiuntivi di ricerca"));
    app.processEvents();
    window.grab().save(QStringLiteral("docs/images/raw_screenshot_options_it.png"));

    QJsonArray itOptBoxes;
    itOptBoxes.append(getItemBox(sensitiveIt1));
    itOptBoxes.append(getItemBox(sensitiveIt2));
    blurMetadata["it_options"] = itOptBoxes;

    // 1.3 Guida Utente Italiana
    {
        GuideDialog guide(QStringLiteral("it"), &window);
        guide.resize(920, 620);
        guide.show();
        app.processEvents();
        guide.grab().save(QStringLiteral("docs/images/raw_screenshot_guide_it.png"));
        guide.close();
    }

    // =========================================================================
    // 2. STATO INGLESE
    // =========================================================================
    langCombo->setCurrentIndex(1); // EN
    app.processEvents();

    searchEdit->setText(QStringLiteral("*.log"));
    folderEdit->setText(QStringLiteral("/var/log"));
    searchBtn->setEnabled(true);
    searchBtn->setText(QStringLiteral("Search"));

    tree->clear();
    addResult(QStringLiteral("bootstrap.log"), QStringLiteral("/var/log"));
    addResult(QStringLiteral("syslog.log"), QStringLiteral("/var/log"));
    addResult(QStringLiteral("dpkg.log"), QStringLiteral("/var/log"));
    addResult(QStringLiteral("pacman.log"), QStringLiteral("/var/log"));
    addResult(QStringLiteral("auth.log"), QStringLiteral("/var/log"));
    addResult(QStringLiteral("faillog"), QStringLiteral("/var/log"));

    auto sensitiveEn1 = addResult(QStringLiteral("audit_tokens.log"), QStringLiteral("/home/john_doe/.audit/10.0.0.42"));
    auto sensitiveEn2 = addResult(QStringLiteral("user_session_keys.log"), QStringLiteral("/home/john_doe/workspace/secrets"));

    addResult(QStringLiteral("lastlog"), QStringLiteral("/var/log"));
    addResult(QStringLiteral("Xorg.0.log"), QStringLiteral("/var/log"));
    addResult(QStringLiteral("wtmp.log"), QStringLiteral("/var/log"));
    addResult(QStringLiteral("journal.log"), QStringLiteral("/var/log"));

    pBar->setRange(0, 100);
    pBar->setValue(100);
    statusLbl->setText(QStringLiteral("Completed: 118 items in 0.02 s"));

    tree->resizeColumnToContents(0);
    if (tree->columnWidth(0) < 320) tree->setColumnWidth(0, 320);

    // 2.1 Schermata principale inglese compatta (opzioni chiuse)
    advWidget->setVisible(false);
    toggleBtn->setText(QString::fromUtf8("▶ ") + QStringLiteral("Additional search parameters"));
    app.processEvents();
    window.grab().save(QStringLiteral("docs/images/raw_screenshot_main_en.png"));

    QJsonArray enBoxes;
    enBoxes.append(getItemBox(sensitiveEn1));
    enBoxes.append(getItemBox(sensitiveEn2));
    blurMetadata["en_main"] = enBoxes;

    // 2.2 Schermata con opzioni espanse inglese
    advWidget->setVisible(true);
    toggleBtn->setText(QString::fromUtf8("▼ ") + QStringLiteral("Additional search parameters"));
    app.processEvents();
    window.grab().save(QStringLiteral("docs/images/raw_screenshot_options_en.png"));

    QJsonArray enOptBoxes;
    enOptBoxes.append(getItemBox(sensitiveEn1));
    enOptBoxes.append(getItemBox(sensitiveEn2));
    blurMetadata["en_options"] = enOptBoxes;

    // 2.3 Guida Utente Inglese
    {
        GuideDialog guide(QStringLiteral("en"), &window);
        guide.resize(920, 620);
        guide.show();
        app.processEvents();
        guide.grab().save(QStringLiteral("docs/images/raw_screenshot_guide_en.png"));
        guide.close();
    }

    // Salva metadati coordinate per il post-processing del blur
    QFile metaFile(QStringLiteral("docs/images/blur_coords.json"));
    if (metaFile.open(QIODevice::WriteOnly)) {
        metaFile.write(QJsonDocument(blurMetadata).toJson(QJsonDocument::Indented));
        metaFile.close();
    }

    qInfo() << "Screenshot e coordinate di blur salvate correttamente!";
    return 0;
}
