#include "themehelper.h"

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QSettings>
#include <QColor>
#include <QDebug>

void ThemeHelper::initTheme()
{
    if (!s_initialized) {
        if (QApplication::style()) {
            s_originalStyleName = QApplication::style()->objectName();
        }
        s_originalPalette = QApplication::palette();
        s_initialized = true;
    }

    QSettings settings(QStringLiteral("PsyTech76"), QStringLiteral("FDg"));
    int savedMode = settings.value(QStringLiteral("ui/themeMode"), static_cast<int>(ThemeMode::Light)).toInt();

    if (savedMode < 0 || savedMode > 2) {
        savedMode = static_cast<int>(ThemeMode::Light);
    }

    applyTheme(static_cast<ThemeMode>(savedMode));
}

void ThemeHelper::applyTheme(ThemeMode mode)
{
    s_currentTheme = mode;

    switch (mode) {
    case ThemeMode::Light: {
        QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
        QApplication::setPalette(createLightPalette());
        break;
    }
    case ThemeMode::Dark: {
        QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
        QApplication::setPalette(createDarkPalette());
        break;
    }
    case ThemeMode::System: {
        if (!s_originalStyleName.isEmpty()) {
            QApplication::setStyle(QStyleFactory::create(s_originalStyleName));
        }
        QApplication::setPalette(s_originalPalette);
        break;
    }
    }
}

void ThemeHelper::setTheme(ThemeMode mode)
{
    QSettings settings(QStringLiteral("PsyTech76"), QStringLiteral("FDg"));
    settings.setValue(QStringLiteral("ui/themeMode"), static_cast<int>(mode));
    applyTheme(mode);
}

ThemeMode ThemeHelper::currentTheme()
{
    return s_currentTheme;
}

QPalette ThemeHelper::createLightPalette()
{
    QPalette pal;

    // Colori finestra e testo
    pal.setColor(QPalette::Window, QColor(239, 239, 239));
    pal.setColor(QPalette::WindowText, QColor(0, 0, 0));

    // Base campi di testo e viste tabellari
    pal.setColor(QPalette::Base, QColor(255, 255, 255));
    pal.setColor(QPalette::AlternateBase, QColor(247, 247, 247));

    // Tooltip
    pal.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
    pal.setColor(QPalette::ToolTipText, QColor(0, 0, 0));

    // Testo generico e pulsanti
    pal.setColor(QPalette::Text, QColor(0, 0, 0));
    pal.setColor(QPalette::Button, QColor(239, 239, 239));
    pal.setColor(QPalette::ButtonText, QColor(0, 0, 0));

    // Evidenziazione e link
    pal.setColor(QPalette::BrightText, QColor(255, 0, 0));
    pal.setColor(QPalette::Link, QColor(41, 128, 185));
    pal.setColor(QPalette::Highlight, QColor(41, 128, 185));
    pal.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    // Stato disabilitato (per pulsanti come Cerca quando il campo è vuoto)
    pal.setColor(QPalette::Disabled, QPalette::Text, QColor(160, 160, 160));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(160, 160, 160));
    pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(160, 160, 160));

    return pal;
}

QPalette ThemeHelper::createDarkPalette()
{
    QPalette pal;

    pal.setColor(QPalette::Window, QColor(45, 45, 45));
    pal.setColor(QPalette::WindowText, QColor(240, 240, 240));

    pal.setColor(QPalette::Base, QColor(30, 30, 30));
    pal.setColor(QPalette::AlternateBase, QColor(38, 38, 38));

    pal.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
    pal.setColor(QPalette::ToolTipText, QColor(0, 0, 0));

    pal.setColor(QPalette::Text, QColor(240, 240, 240));
    pal.setColor(QPalette::Button, QColor(50, 50, 50));
    pal.setColor(QPalette::ButtonText, QColor(240, 240, 240));

    pal.setColor(QPalette::BrightText, QColor(255, 0, 0));
    pal.setColor(QPalette::Link, QColor(52, 152, 219));
    pal.setColor(QPalette::Highlight, QColor(52, 152, 219));
    pal.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

    pal.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
    pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));

    return pal;
}
