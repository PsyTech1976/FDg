#ifndef GUIDEDIALOG_H
#define GUIDEDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QUrl>

/**
 * @brief Browser minimale integrato per la visualizzazione della guida HTML dell'applicazione.
 *
 * NOTE ARCHITETTURALI PER FUTURE REVISIONI (UMANO O IA):
 * - Perché QTextBrowser e non QWebEngineView:
 *   L'utilizzo di QWebEngineView (Chromium) comporterebbe un incremento del pacchetto
 *   AppImage di oltre 150 MB, oltre a introdurre complesse dipendenze grafiche (GPU, sandbox,
 *   fontconfig, nss) che minerebbero la compatibilità cross-distribution universale
 *   tra Debian 12 (stabile) e Arch Linux (rolling). QTextBrowser fa parte di QtWidgets,
 *   è estremamente leggero, supporta nativamente HTML4/CSS parziale, tabelle, formattazione,
 *   e si integra istantaneamente con le risorse binarie incorporate in Qt (qrc:/).
 * - Portabilità totale:
 *   I file HTML della guida (guide_it.html, guide_en.html, etc.) sono inclusi nel file
 *   di risorse resources.qrc e incorporati nel binario eseguibile; non necessitano
 *   quindi di alcun file esterno sul disco, rendendo l'AppImage autonoma al 100%.
 * - Navigazione multilingua contestuale:
 *   La finestra supporta il cambio rapido della lingua sia tramite il menu a tendina
 *   superiore dotato di bandiere grafiche SVG/PNG, sia tramite i link ipertestuali
 *   presenti all'interno di ogni singola pagina HTML.
 */
class GuideDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Costruttore della finestra browser della guida.
     * @param initialLocale Codice lingua iniziale (es. "it", "en", "de", "es", "fr").
     * @param parent Widget genitore (tipicamente MainWindow).
     */
    explicit GuideDialog(const QString &initialLocale = QStringLiteral("it"), QWidget *parent = nullptr);
    ~GuideDialog() override = default;

    /**
     * @brief Imposta e carica la pagina della guida corrispondente al locale specificato.
     * Sincronizza automaticamente il ComboBox superiore delle lingue senza innescare loop di segnali.
     * @param localeCode Codice lingua da caricare (it, en, de, es, fr).
     */
    void setLanguage(const QString &localeCode);

private slots:
    /**
     * @brief Slot invocato al cambio selezione manuale dal ComboBox delle lingue.
     * @param index Indice dell'elemento selezionato.
     */
    void onLanguageChanged(int index);

    /**
     * @brief Slot invocato quando il QTextBrowser naviga verso una nuova URL.
     * Consente di rilevare la lingua della nuova pagina e aggiornare il ComboBox.
     * @param url Nuovo indirizzo caricato.
     */
    void onSourceChanged(const QUrl &url);

private:
    /**
     * @brief Costruisce e configura l'interfaccia grafica del browser minimale.
     */
    void setupUi();

    QTextBrowser *m_browser;         ///< Visualizzatore HTML nativo QtWidgets
    QComboBox *m_langCombo;          ///< Selettore lingua con bandiere grafiche
    QPushButton *m_backButton;       ///< Pulsante navigazione indietro
    QPushButton *m_forwardButton;    ///< Pulsante navigazione avanti
    QPushButton *m_homeButton;       ///< Pulsante per tornare all'inizio della guida
    QString m_currentLocale;         ///< Codice lingua attualmente visualizzato
};

#endif // GUIDEDIALOG_H
