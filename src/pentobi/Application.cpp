//-----------------------------------------------------------------------------
/** @file pentobi/Application.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
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
    return false;
}

//-----------------------------------------------------------------------------
