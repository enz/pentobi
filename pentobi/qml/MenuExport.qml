//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuExport.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "." as Pentobi

Pentobi.Menu {
    title: addShortcut(qsTr("Export"),
                       //: Keyboard shortcut for menu Export. Leave empty for no shortcut.
                       qsTr("E"))

    Action {
        text: addShortcut(qsTr("Image…"),
                          //: Keyboard shortcut for menu item Image. Leave empty for no shortcut.
                          qsTr("M"))
        onTriggered: exportImageDialog.open()
    }
    Action {
        text: addShortcut(qsTr("ASCII Art…"),
                          //: Keyboard shortcut for menu item ASCII Art. Leave empty for no shortcut.
                          qsTr("A"))
        onTriggered: {
            var dialog = asciiArtSaveDialog.get()
            dialog.name = gameModel.suggestFileName(folder, "txt")
            dialog.selectNameFilter(0)
            dialog.open()
        }
    }
}
