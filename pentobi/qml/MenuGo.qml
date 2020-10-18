//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGo.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("Go")

    Pentobi.MenuItem {
        action: actionGotoMove
    }
    Pentobi.MenuItem {
        action: actionBackToMainVar
    }
    Pentobi.MenuItem {
        action: actionBeginningOfBranch
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actionNextComment
    }
}
