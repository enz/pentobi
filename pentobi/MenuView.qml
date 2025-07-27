//-----------------------------------------------------------------------------
/** @file pentobi/MenuView.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    title: qsTr("View")

    MenuItem {
        text: qsTr("Appearance")
        onTriggered: appearanceDialog.open()
    }
    MenuItem {
        visible: isDesktop
        // Invisible menu item still use space in Qt 5.11
        height: visible ? implicitHeight : 0
        text: qsTr("Toolbar")
        checkable: true
        checked: rootWindow.showToolBar
        onTriggered: rootWindow.showToolBar = checked
    }
    MenuItem {
        action: actionComment
    }
    MenuItem {
        action: actionFullscreen
    }
}
