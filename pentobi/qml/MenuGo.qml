//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGo.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "." as Pentobi

Pentobi.Menu {
    title: addShortcut(qsTr("Go"),
                       //: Keyboard shortcut for menu Go. Leave empty for no shortcut.
                       qsTr("O"))

    Pentobi.MenuItem {
        action: actionGotoMove
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Go/Move Number. Leave empty for no shortcut.
                          qsTr("N"))
    }
    Pentobi.MenuItem {
        action: actionBackToMainVar
        text: addShortcut(actionBackToMainVar.text,
                          //: Keyboard shortcut for menu item Go/Main Variation. Leave empty for no shortcut.
                          qsTr("M"))
    }
    Pentobi.MenuItem {
        action: actionBeginningOfBranch
        text: addShortcut(actionBeginningOfBranch.text,
                          //: Keyboard shortcut for menu item Beginning Of Branch. Leave empty for no shortcut.
                          qsTr("B"))
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actionNextComment
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Next Comment. Leave empty for no shortcut.
                          qsTr("C"))
    }
}
