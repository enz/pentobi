//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGo.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3

PentobiMenu {
    title: qsTr("Go")

    PentobiMenuItem {
        action: actionGotoMove
    }
    PentobiMenuItem {
        action: actionBackToMainVar
    }
    PentobiMenuItem {
        action: actionBeginningOfBranch
    }
    MenuSeparator { }
    PentobiMenuItem {
        action: actionNextComment
    }
}
