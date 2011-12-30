//-----------------------------------------------------------------------------
/** @file libpentobi_gui/HelpWindow.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_HELP_WINDOW_H
#define LIBPENTOBI_GUI_HELP_WINDOW_H

#include <QtGui>

//-----------------------------------------------------------------------------

class HelpWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    /** Find the main page for a given language.
        Assumes that the translations are in subdirectories of a given
        directory. The subdirectories use either the language code as the name
        (e.g. "en") or the language code followed by the country code separated
        by an underscore (e.g. "en_US"). "en" must always exist and is returned
        as the default if no translation for a given language exists.
        @param dir The directory
        @param file The file name of the main page
        @param locale The language code followed by the country code separated
        by an underscore (as returned by QLocale::system().name())
        @return The full path of the main page. */
    static QString findMainPage(QString dir, QString file, QString locale);

    HelpWindow(QWidget* parent, const QString& mainPage);

    QSize sizeHint() const;

protected:
    void closeEvent(QCloseEvent* event);

private:
    QUrl m_mainPageUrl;

    QAction* m_actionHome;

private slots:
    void handleSourceChanged(const QUrl& src);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_HELP_WINDOW_H
