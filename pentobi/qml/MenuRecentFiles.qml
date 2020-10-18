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
    enabled: gameModel.recentFiles.length > 0
    relativeWidth: 23

    function getText(recentFiles, index) {
        if (index >= recentFiles.length)
            return ""
        var text = recentFiles[index]
        return text.substring(text.lastIndexOf("/") + 1)
    }

    // Instantiator in Menu doesn't work reliably with Qt 5.11 or 5.12.0 alpha
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 0
        // Invisible menu item still use space in Qt 5.11
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 0)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[0])
    }
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 1
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 1)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[1])
    }
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 2
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 2)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[2])
    }
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 3
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 3)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[3])
    }
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 4
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 4)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[4])
    }
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 5
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 5)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[5])
    }
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 6
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 6)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[6])
    }
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 7
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 7)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[7])
    }
    Pentobi.MenuItem {
        visible: gameModel.recentFiles.length > 8
        height: visible ? implicitHeight : 0
        text: getText(gameModel.recentFiles, 8)
        onTriggered: Logic.openRecentFile(gameModel.recentFiles[8])
    }

    Pentobi.MenuSeparator { }
    Action {
        //: Menu item for clearing the recent files list
        text: qsTr("Clear List")
        onTriggered: Qt.callLater(function() { // QTBUG-69682
            gameModel.clearRecentFiles()
        })
    }
}
