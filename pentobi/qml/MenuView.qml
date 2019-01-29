//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuView.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import QtQuick.Window 2.1
import "." as Pentobi

Pentobi.Menu {
    title: addShortcut(qsTr("View"),
                       //: Keyboard shortcut for menu View. Leave empty for no shortcut.
                       qsTr("V"))

    Action {
        text: addShortcut(qsTr("Appearance"),
                          //: Keyboard shortcut for menu Appearance. Leave empty for no shortcut.
                          qsTr("A"))
        onTriggered: appearanceDialog.open()
    }
    Pentobi.MenuItem {
        visible: isDesktop
        // Invisible menu item still use space in Qt 5.11
        height: visible ? implicitHeight : 0
        text: addShortcut(qsTr("Toolbar"),
                          //: Keyboard shortcut for menu item View/Toolbar. Leave empty for no shortcut.
                          qsTr("T"))
        checkable: true
        checked: rootWindow.showToolBar
        onTriggered: rootWindow.showToolBar = checked
    }
    Pentobi.MenuItem {
        action: actionComment
        text: addShortcut(actionComment.text,
                          //: Keyboard shortcut for menu item View/Comment. Leave empty for no shortcut.
                          qsTr("C"))
    }
    Pentobi.MenuItem {
        action: actionFullscreen
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Fullscreen. Leave empty for no shortcut.
                          qsTr("F"))
    }
}
