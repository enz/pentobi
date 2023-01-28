//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuView.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls
import QtQuick.Window

PentobiMenu {
    title: qsTr("View")

    Action {
        text: qsTr("Appearance")
        onTriggered: appearanceDialog.open()
    }
    MenuItem {
        visible: isDesktop
        // Invisible menu item still use space in Qt 5.11
        height: visible ? implicitHeight : 0
        text: (! checkable && checked ? "✓  " : "") + qsTr("Toolbar")
        checkable: globalStyle !== "Basic"
        checked: rootWindow.showToolBar
        onTriggered: {
            if (! checkable)
                checked = ! checked
            rootWindow.showToolBar = checked
        }
    }
    MenuItem {
        text: (! checkable && checked ? "✓  " : "") + actionComment.text
        checkable: globalStyle !== "Basic"
        checked: actionComment.checked
        onTriggered: actionComment.trigger()
    }
    MenuItem {
        text: (! checkable && checked ? "✓  " : "") + actionFullscreen.text
        checkable: globalStyle !== "Basic"
        checked: actionFullscreen.checked
        onTriggered: actionFullscreen.trigger()
    }
}
