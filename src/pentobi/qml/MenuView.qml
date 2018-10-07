//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuView.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import QtQuick.Window 2.1
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("View"),
                       //: Mnemonic for menu View. Leave empty for no mnemonic.
                       qsTr("V"))

    Action {
        text: addMnemonic(qsTr("Appearance"),
                          //: Mnemonic for menu Appearance. Leave empty for no mnemonic.
                          qsTr("A"))
        onTriggered: appearanceDialog.open()
    }
    Pentobi.MenuItem {
        visible: isDesktop
        // Invisible menu item still use space in Qt 5.11
        height: visible ? implicitHeight : 0
        text: addMnemonic(qsTr("Toolbar"),
                          //: Mnemonic for menu item View/Toolbar. Leave empty for no mnemonic.
                          qsTr("T"))
        checkable: true
        checked: rootWindow.showToolBar
        onTriggered: rootWindow.showToolBar = checked
    }
    Pentobi.MenuItem {
        action: actionComment
        text: addMnemonic(actionComment.text,
                          //: Mnemonic for menu item View/Comment. Leave empty for no mnemonic.
                          qsTr("C"))
    }
    Pentobi.MenuItem {
        action: actionFullscreen
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Fullscreen. Leave empty for no mnemonic.
                          qsTr("F"))
    }
}
