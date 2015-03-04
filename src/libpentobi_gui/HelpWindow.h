//-----------------------------------------------------------------------------
/** @file libpentobi_gui/HelpWindow.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_HELP_WINDOW_H
#define LIBPENTOBI_GUI_HELP_WINDOW_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QMainWindow>
#include <QUrl>

//-----------------------------------------------------------------------------

class HelpWindow
    : public QMainWindow
{
    Q_OBJECT

public:
    /** Find the main page for a given language.
        Assumes that the layout of the help directory is according to
        http://www.freedesktop.org/wiki/Specifications/help-spec/
        @param helpDir The help directory.
        @param appName The subdirectory name for the application.
        @return The full path of index.html. */
    static QString findMainPage(QString helpDir, QString appName);

    HelpWindow(QWidget* parent, const QString& title, const QString& mainPage);

    QSize sizeHint() const override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QUrl m_mainPageUrl;

    QAction* m_actionHome;

private slots:
    void handleSourceChanged(const QUrl& src);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_HELP_WINDOW_H
