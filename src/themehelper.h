#ifndef THEMEHELPER_H
#define THEMEHELPER_H

#include <QString>
#include <QPalette>

/**
 * @brief Modalità del tema grafico per l'applicazione FDg.
 */
enum class ThemeMode {
    Light = 0,   ///< Chiaro (Predefinito, Fusion con palette uniforme identica alla documentazione)
    Dark = 1,    ///< Scuro (Fusion con palette dark ad alto contrasto ed ergonomia visiva)
    System = 2   ///< Sistema (Ripristina lo stile e la palette nativi dell'ambiente desktop)
};

/**
 * @brief Gestore centralizzato per lo stile grafico e la palette dell'applicazione.
 *
 * Risolve la discrepanza visiva tra ambienti desktop differenti (es. KDE Plasma con Breeze
 * scuro vs GNOME con Adwaita) assicurando che chi compila ed esegue il programma da sorgenti
 * o AppImage ottenga di default l'esatto stile Fusion mostrato nella documentazione e negli screenshot.
 */
class ThemeHelper
{
public:
    static void initTheme();
    static void applyTheme(ThemeMode mode);
    static void setTheme(ThemeMode mode);
    static ThemeMode currentTheme();

private:
    static QPalette createLightPalette();
    static QPalette createDarkPalette();

    static inline ThemeMode s_currentTheme = ThemeMode::Light;
    static inline QString s_originalStyleName;
    static inline QPalette s_originalPalette;
    static inline bool s_initialized = false;
};

#endif // THEMEHELPER_H
