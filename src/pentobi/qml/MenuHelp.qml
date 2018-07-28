//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuHelp.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Help"),
                       //: Mnemonic for menu Help. Leave empty for no mnemonic.
                       qsTr("H"))

    Pentobi.MenuItem {
        action: actions.actionHelp
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Pentobi Help. Leave empty for no mnemonic.
                          qsTr("P"))
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("About Pentobi"),
                          //: Mnemonic for menu item About Pentobi. Leave empty for no mnemonic.
                          qsTr("A"))
        onTriggered: aboutDialog.open()
    }
}
