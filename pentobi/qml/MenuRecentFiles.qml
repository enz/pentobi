//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuRecentFiles.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.4
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("Open Recent")
    enabled: recentFiles.entries.length > 0
    relativeWidth: 19

    function getText(recentFiles, index) {
        if (index >= recentFiles.length)
            return ""
        var text = recentFiles[index].displayName
        if (! text || text === "") {
            text = recentFiles[index].file
            text = text.substring(text.lastIndexOf("/") + 1)
        }
        text = text.replace("\.blksgf", "")
        return text
    }

    // Instantiator in Menu doesn't work reliably with Qt 5.11 or 5.12.0 alpha
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 0
        // Invisible menu item still use space in Qt 5.11
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 0)
        onTriggered: Logic.openRecentFile(recentFiles.entries[0].file,
                                          recentFiles.entries[0].displayName)
    }
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 1
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 1)
        onTriggered: Logic.openRecentFile(recentFiles.entries[1].file,
                                          recentFiles.entries[1].displayName)
    }
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 2
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 2)
        onTriggered: Logic.openRecentFile(recentFiles.entries[2].file,
                                          recentFiles.entries[2].displayName)
    }
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 3
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 3)
        onTriggered: Logic.openRecentFile(recentFiles.entries[3].file,
                                          recentFiles.entries[3].displayName)
    }
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 4
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 4)
        onTriggered: Logic.openRecentFile(recentFiles.entries[4].file,
                                          recentFiles.entries[4].displayName)
    }
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 5
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 5)
        onTriggered: Logic.openRecentFile(recentFiles.entries[5].file,
                                          recentFiles.entries[5].displayName)
    }
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 6
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 6)
        onTriggered: Logic.openRecentFile(recentFiles.entries[6].file,
                                          recentFiles.entries[6].displayName)
    }
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 7
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 7)
        onTriggered: Logic.openRecentFile(recentFiles.entries[7].file,
                                          recentFiles.entries[7].displayName)
    }
    Pentobi.MenuItem {
        visible: recentFiles.entries.length > 8
        height: visible ? implicitHeight : 0
        text: getText(recentFiles.entries, 8)
        onTriggered: Logic.openRecentFile(recentFiles.entries[8].file,
                                          recentFiles.entries[8].displayName)
    }
    MenuSeparator { }
    Action {
        //: Menu item for clearing the recent files list
        text: qsTr("Clear List")
        // recentFiles.clear() must be called after menu is closed because it
        // modifies the menu and otherwise the menu stays visible (tested with
        // Qt 5.15.1)
        onTriggered: Qt.callLater(function() {
            recentFiles.clear(gameModel.file)
        })
    }
}
