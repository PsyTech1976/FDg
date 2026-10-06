#include "guidedialog.h"
#include <QIcon>
#include <QDesktopServices>
#include <QFileInfo>

GuideDialog::GuideDialog(const QString &initialLocale, QWidget *parent)
    : QDialog(parent)
    , m_currentLocale(initialLocale.isEmpty() ? QStringLiteral("it") : initialLocale)
{
    setupUi();
    setLanguage(m_currentLocale);
}

void GuideDialog::setupUi()
{
    setWindowTitle(tr("FDg - Guida e Manuale Utente"));
    setWindowIcon(QIcon(QStringLiteral(":/icons/appicon.png")));
    resize(860, 640);
    setMinimumSize(600, 450);

    // Layout principale verticale
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    // Barra superiore di navigazione
    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->setSpacing(6);

    m_backButton = new QPushButton(tr("◀ Indietro"), this);
    m_backButton->setToolTip(tr("Torna alla pagina precedente (Alt+Freccia Sinistra)"));
    m_backButton->setEnabled(false);

    m_forwardButton = new QPushButton(tr("Avanti ▶"), this);
    m_forwardButton->setToolTip(tr("Vai alla pagina successiva (Alt+Freccia Destra)"));
    m_forwardButton->setEnabled(false);

    m_homeButton = new QPushButton(tr("🏠 Inizio Guida"), this);
    m_homeButton->setToolTip(tr("Torna all'inizio della guida per la lingua corrente"));

    // Selettore lingua
    m_langCombo = new QComboBox(this);
    m_langCombo->setIconSize(QSize(20, 15));
    m_langCombo->addItem(QIcon(QStringLiteral(":/icons/flags/it.png")), QStringLiteral("Italiano"), QStringLiteral("it"));
    m_langCombo->addItem(QIcon(QStringLiteral(":/icons/flags/en.png")), QStringLiteral("English"), QStringLiteral("en"));
    m_langCombo->addItem(QIcon(QStringLiteral(":/icons/flags/de.png")), QStringLiteral("Deutsch"), QStringLiteral("de"));
    m_langCombo->addItem(QIcon(QStringLiteral(":/icons/flags/es.png")), QStringLiteral("Español"), QStringLiteral("es"));
    m_langCombo->addItem(QIcon(QStringLiteral(":/icons/flags/fr.png")), QStringLiteral("Français"), QStringLiteral("fr"));

    navLayout->addWidget(m_backButton);
    navLayout->addWidget(m_forwardButton);
    navLayout->addWidget(m_homeButton);
    navLayout->addSpacing(16);
    navLayout->addWidget(m_langCombo);
    navLayout->addStretch(1);

    QPushButton *closeBtn = new QPushButton(tr("Chiudi"), this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    navLayout->addWidget(closeBtn);

    mainLayout->addLayout(navLayout);

    // Browser HTML integrato
    m_browser = new QTextBrowser(this);
    m_browser->setOpenExternalLinks(false);
    m_browser->setSearchPaths(QStringList() << QStringLiteral(":/help") << QStringLiteral(":/icons/flags"));

    mainLayout->addWidget(m_browser, 1);

    // Connessioni di navigazione
    connect(m_backButton, &QPushButton::clicked, m_browser, &QTextBrowser::backward);
    connect(m_forwardButton, &QPushButton::clicked, m_browser, &QTextBrowser::forward);
    connect(m_browser, &QTextBrowser::backwardAvailable, m_backButton, &QPushButton::setEnabled);
    connect(m_browser, &QTextBrowser::forwardAvailable, m_forwardButton, &QPushButton::setEnabled);
    connect(m_browser, &QTextBrowser::sourceChanged, this, &GuideDialog::onSourceChanged);

    connect(m_homeButton, &QPushButton::clicked, this, [this]() {
        setLanguage(m_currentLocale);
    });

    connect(m_langCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GuideDialog::onLanguageChanged);

    // Gestione dei link cliccati
    connect(m_browser, &QTextBrowser::anchorClicked, this, [this](const QUrl &link) {
        QString scheme = link.scheme();
        if (scheme == QStringLiteral("http") || scheme == QStringLiteral("https") || scheme == QStringLiteral("mailto")) {
            QDesktopServices::openUrl(link);
        } else {
            // Link interno (es. guide_en.html)
            QString fileName = QFileInfo(link.path()).fileName();
            if (fileName.startsWith(QStringLiteral("guide_")) && fileName.endsWith(QStringLiteral(".html"))) {
                QString code = fileName.mid(6, fileName.length() - 11);
                m_currentLocale = code;
                for (int i = 0; i < m_langCombo->count(); ++i) {
                    if (m_langCombo->itemData(i).toString() == code) {
                        m_langCombo->blockSignals(true);
                        m_langCombo->setCurrentIndex(i);
                        m_langCombo->blockSignals(false);
                        break;
                    }
                }
            }
            m_browser->setSource(QUrl(QStringLiteral("qrc:/help/") + fileName));
        }
    });
}

void GuideDialog::setLanguage(const QString &localeCode)
{
    m_currentLocale = localeCode;
    QString targetUrl = QStringLiteral("qrc:/help/guide_%1.html").arg(localeCode);

    // Sincronizza il selettore delle lingue
    for (int i = 0; i < m_langCombo->count(); ++i) {
        if (m_langCombo->itemData(i).toString() == localeCode) {
            m_langCombo->blockSignals(true);
            m_langCombo->setCurrentIndex(i);
            m_langCombo->blockSignals(false);
            break;
        }
    }

    m_browser->setSource(QUrl(targetUrl));
}

void GuideDialog::onLanguageChanged(int index)
{
    QString code = m_langCombo->itemData(index).toString();
    if (!code.isEmpty()) {
        m_currentLocale = code;
        m_browser->setSource(QUrl(QStringLiteral("qrc:/help/guide_%1.html").arg(code)));
    }
}

void GuideDialog::onSourceChanged(const QUrl &url)
{
    QString fileName = QFileInfo(url.path()).fileName();
    if (fileName.startsWith(QStringLiteral("guide_")) && fileName.endsWith(QStringLiteral(".html"))) {
        QString code = fileName.mid(6, fileName.length() - 11);
        m_currentLocale = code;
        for (int i = 0; i < m_langCombo->count(); ++i) {
            if (m_langCombo->itemData(i).toString() == code) {
                m_langCombo->blockSignals(true);
                m_langCombo->setCurrentIndex(i);
                m_langCombo->blockSignals(false);
                break;
            }
        }
    }
}
