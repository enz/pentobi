//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuView.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("View"),
                       //: Mnemonic for menu View. Leave empty for no mnemonic.
                       qsTr("V"))

    Action {
        text: addMnemonic(qsTr("Appearance..."),
                          //: Mnemonic for menu Appearance. Leave empty for no mnemonic.
                          qsTr("A"))
        onTriggered: appearanceDialog.open()
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.actionFullscreen
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Fullscreen. Leave empty for no mnemonic.
                          qsTr("F"))
    }
}
