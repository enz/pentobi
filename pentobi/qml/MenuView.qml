//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuView.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import QtQuick.Window 2.1
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("View")

    Action {
        text: qsTr("Appearance")
        onTriggered: appearanceDialog.open()
    }
    Pentobi.MenuItem {
        visible: isDesktop
        // Invisible menu item still use space in Qt 5.11
        height: visible ? implicitHeight : 0
        text: qsTr("Toolbar")
        checkable: true
        checked: rootWindow.showToolBar
        onTriggered: rootWindow.showToolBar = checked
    }
    Pentobi.MenuItem {
        action: actionComment
    }
    Pentobi.MenuItem {
        action: actionFullscreen
    }
}
