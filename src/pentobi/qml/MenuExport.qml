//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuExport.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Export"),
                       //: Mnemonic for menu Export. Leave empty for no mnemonic.
                       qsTr("E"))

    Action {
        text: addMnemonic(qsTr("Image…"),
                          //: Mnemonic for menu item Image. Leave empty for no mnemonic.
                          qsTr("M"))
        onTriggered: exportImageDialog.open()
    }
    Action {
        text: addMnemonic(qsTr("ASCII Art…"),
                          //: Mnemonic for menu item ASCII Art. Leave empty for no mnemonic.
                          qsTr("A"))
        onTriggered: {
            var dialog = asciiArtSaveDialog.get()
            dialog.name = gameModel.suggestFileName(folder, "txt")
            dialog.selectNameFilter(0)
            dialog.open()
        }
    }
}
