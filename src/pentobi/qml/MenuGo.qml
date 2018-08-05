//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGo.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Go"),
                       //: Mnemonic for menu Go. Leave empty for no mnemonic.
                       qsTr("O"))

    Pentobi.MenuItem {
        action: actions.gotoMove
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Go/Move Number. Leave empty for no mnemonic.
                          qsTr("N"))
    }
    Pentobi.MenuItem {
        action: actions.backToMainVar
        text: addMnemonic(actions.backToMainVar.text,
                          //: Mnemonic for menu item Go/Main Variation. Leave empty for no mnemonic.
                          qsTr("M"))
    }
    Pentobi.MenuItem {
        action: actions.beginningOfBranch
        text: addMnemonic(actions.beginningOfBranch.text,
                          //: Mnemonic for menu item Beginning Of Branch. Leave empty for no mnemonic.
                          qsTr("B"))
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.nextComment
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Next Comment. Leave empty for no mnemonic.
                          qsTr("C"))
    }
}
