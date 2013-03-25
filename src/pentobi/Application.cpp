//-----------------------------------------------------------------------------
/** @file pentobi/Application.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Application.h"

#include "ShowMessage.h"
#include "libboardgame_sys/Compiler.h"

using namespace std;
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
        showFatal(QString::fromLocal8Bit(detailedText.c_str()));
    }
    catch (...)
    {
        showFatal("Unknown exception");
    }
    return false;
}

//-----------------------------------------------------------------------------
