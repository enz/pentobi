//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuRecentFiles.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.2
import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    id: root

    title: addMnemonic(qsTr("Open Recent"),
                       //: Mnemonic for menu Open Recent. Leave empty for no mnemonic.
                       qsTr("P"))
    enabled: gameModel.recentFiles.length > 0

    Instantiator {
        model: gameModel.recentFiles
        onObjectAdded: root.insertItem(index, object)
        onObjectRemoved: root.removeItem(object)

        Pentobi.MenuItem {
            text: {
                var result = modelData.substring(modelData.lastIndexOf("/") + 1)
                if (isDesktop)
                    //: Format in recent files menu. First argument is the
                    //: file number, second argument the file name.
                    result = addMnemonic(qsTr("%1. %2").arg(index + 1).arg(result),
                                         (index + 1).toString())
                return result
            }
            onTriggered: Logic.openRecentFile(modelData)
        }
    }
    Pentobi.MenuSeparator { }
    Action {
        //: Menu item for clearing the recent files list
        text: addMnemonic(qsTr("Clear List"),
                          //: Mnemonic for menu item Recent Files/Clear List. Leave empty for no mnemonic.
                          qsTr("C"))
        onTriggered: gameModel.clearRecentFiles()
    }
}
