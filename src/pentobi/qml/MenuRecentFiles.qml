//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuRecentFiles.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.4
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Open Recent"),
                       //: Mnemonic for menu Open Recent. Leave empty for no mnemonic.
                       qsTr("P"))
    enabled: gameModel.recentFiles.length > 0

    function getText(recentFiles, index) {
        if (index >= recentFiles.length)
            return ""
        var text = recentFiles[index]
        text = text.substring(text.lastIndexOf("/") + 1)
        if (isDesktop)
            //: Format in recent files menu. First argument is the
            //: file number, second argument the file name.
            text = addMnemonic(qsTr("%1. %2").arg(index + 1).arg(text),
                               (index + 1).toString())
        return text
    }

    // Using Instantiator doesn't work reliably with Qt 5.11 or 5.12.0 alpha
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
        text: addMnemonic(qsTr("Clear List"),
                          //: Mnemonic for menu item Recent Files/Clear List. Leave empty for no mnemonic.
                          qsTr("C"))
        onTriggered: Qt.callLater(function() { // QTBUG-69682
            gameModel.clearRecentFiles()
        })
    }
}
