//-----------------------------------------------------------------------------
/** @file pentobi/MenuRecentFiles.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls
import "main.js" as Logic

PentobiMenu {
    title: qsTr("Open Recent")
    relativeWidth: 19

    function getText(recentFiles, index) {
        if (index >= recentFiles.length)
            return ""
        var text = recentFiles[index].displayName
        if (! text || text === "") {
            text = recentFiles[index].file
            text = text.substring(text.lastIndexOf("/") + 1)
        }
        text = text.replace(/\.blksgf$/i, "")
        return text
    }
    function openFile(index) {
        Logic.openRecentFile(recentFiles.entries[index].file,
                             recentFiles.entries[index].displayName)
    }

    // Instantiator in Menu doesn't work reliably with Qt 5.11 or 5.12.0 alpha
    MenuItem {
        visible: recentFiles.entries.length > 0
        // Invisible menu item still use space in Qt 5.11
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 0)
        onTriggered: openFile(0)
    }
    MenuItem {
        visible: recentFiles.entries.length > 1
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 1)
        onTriggered: openFile(1)
    }
    MenuItem {
        visible: recentFiles.entries.length > 2
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 2)
        onTriggered: openFile(2)
    }
    MenuItem {
        visible: recentFiles.entries.length > 3
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 3)
        onTriggered: openFile(3)
    }
    MenuItem {
        visible: recentFiles.entries.length > 4
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 4)
        onTriggered: openFile(4)
    }
    MenuItem {
        visible: recentFiles.entries.length > 5
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 5)
        onTriggered: openFile(5)
    }
    MenuItem {
        visible: recentFiles.entries.length > 6
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 6)
        onTriggered: openFile(6)
    }
    MenuItem {
        visible: recentFiles.entries.length > 7
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 7)
        onTriggered: openFile(7)
    }
    MenuItem {
        visible: recentFiles.entries.length > 8
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 8)
        onTriggered: openFile(8)
    }
    MenuSeparator {
        visible: recentFiles.entries.length > 0
    }
    MenuItem {
        //: Menu item for clearing the recent files list
        text: qsTr("Clear List")
        enabled: recentFiles.entries.length > 0
        // recentFiles.clear() must be called after menu is closed because it
        // modifies the menu and otherwise the menu stays visible (tested with
        // Qt 5.15.1)
        onTriggered: Qt.callLater(function() {
            recentFiles.clear(gameModel.file)
        })
    }
}
