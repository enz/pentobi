//-----------------------------------------------------------------------------
/** @file pentobi/MenuRecentFiles.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls
import QtQml.Models
import "main.js" as Logic

PentobiMenu {
    id: root

    title: qsTr("Open Recent")
    relativeWidth: 19
    enabled: instantiator.count > 0

    Instantiator {
        id: instantiator

        model: recentFiles.entries
        delegate: MenuItem {
            text: {
                var text = modelData.displayName
                if (! text || text === "") {
                    text = modelData.file
                    text = text.substring(text.lastIndexOf("/") + 1)
                }
                text = text.replace(/\.blksgf$/i, "")
                return text
            }
            onTriggered:
                Logic.openRecentFile(modelData.file, modelData.displayName)
        }
        onObjectAdded: (index, obj) => root.insertItem(index, obj)
        onObjectRemoved: (index, obj) => root.removeItem(obj)
    }
    MenuSeparator { }
    MenuItem {
        //: Menu item for clearing the recent files list
        text: qsTr("Clear List")
        // Call recentFiles.clear() after menu is closed because it modifies
        // the menu and otherwise the menu stays visible (Qt 5.15.1)
        onTriggered: Qt.callLater(function() {
            recentFiles.clear(gameModel.file)
        })
    }
}
