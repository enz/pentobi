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
                          //: Mnemonic for menu item Go to Move. Leave empty for no mnemonic.
                          qsTr("G"))
    }
    Pentobi.MenuItem {
        enabled: actions.backToMainVar.enabled
        text: addMnemonic(actions.backToMainVar.text,
                          //: Mnemonic for menu item Back to Main Variation. Leave empty for no mnemonic.
                          qsTr("M"))
        shortcut: actions.backToMainVar.shortcut
        onTriggered: {
            actions.backToMainVar.onTriggered()
            dismiss() // QTBUG-69682
        }
    }
    Pentobi.MenuItem {
        enabled: actions.beginningOfBranch.enabled
        text: addMnemonic(actions.beginningOfBranch.text,
                          //: Mnemonic for menu item Beginning Of Branch. Leave empty for no mnemonic.
                          qsTr("B"))
        shortcut: actions.beginningOfBranch.shortcut
        onTriggered: {
            actions.beginningOfBranch.onTriggered()
            dismiss() // QTBUG-69682
        }
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.nextComment
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Next Comment. Leave empty for no mnemonic.
                          qsTr("C"))
    }
}
