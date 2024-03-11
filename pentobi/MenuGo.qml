//-----------------------------------------------------------------------------
/** @file pentobi/MenuGo.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    title: qsTr("Go")

    MenuItem {
        action: actionGotoMove
    }
    MenuItem {
        action: actionBackToMainVar
    }
    MenuItem {
        action: actionBeginningOfBranch
    }
    MenuSeparator { }
    MenuItem {
        action: actionNextComment
    }
}
