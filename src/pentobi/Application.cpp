//-----------------------------------------------------------------------------
/** @file pentobi/Application.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Application.h"

#include <QMessageBox>
#include "MainWindow.h"
#include "libboardgame_sys/Compiler.h"

using libboardgame_sys::get_type_name;

//-----------------------------------------------------------------------------

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
{
}

bool Application::notify(QObject* receiver, QEvent* event)
{
    try
    {
        return QApplication::notify(receiver, event);
    }
    catch (const exception& e)
    {
        string detailedText = get_type_name(e) + ": " + e.what();
        showError(detailedText.c_str());
    }
    catch (...)
    {
        showError("");
    }
    return false;
}

void Application::showError(const QString& detailedText)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Pentobi"));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(tr("An unexpected error occurred."));
    msgBox.setInformativeText("<html>" +
                              tr("Please report this error together with any"
                                 " details available with the button below"
                                 " and other context information at the Pentobi"
                                 " <a href=\"http://sourceforge.net/tracker/?atid=2298094&group_id=566594\">bug tracker</a>."));
    if (detailedText.isEmpty())
        msgBox.setDetailedText(tr("No detailed information is available"));
    else
        msgBox.setDetailedText(detailedText);
    msgBox.exec();
}

//-----------------------------------------------------------------------------
