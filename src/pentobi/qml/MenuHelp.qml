//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuHelp.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Help"),
                       //: Mnemonic for menu Help. Leave empty for no mnemonic.
                       qsTr("H"))

    Pentobi.MenuItem {
        action: actions.help
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Pentobi Help. Leave empty for no mnemonic.
                          qsTr("P"))
    }
    Action {
        text: addMnemonic(qsTr("Report Bug"),
                          //: Mnemonic for menu item Report Bug. Leave empty for no mnemonic.
                          qsTr("B"))
        onTriggered: Qt.openUrlExternally("https://sourceforge.net/p/pentobi/bugs/")
    }
    Action {
        text: addMnemonic(qsTr("About Pentobi"),
                          //: Mnemonic for menu item About Pentobi. Leave empty for no mnemonic.
                          qsTr("A"))
        onTriggered: aboutDialog.open()
    }
}
