//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuExport.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    title: qsTr("Export")

    Action {
        text: qsTr("Image…")
        onTriggered: exportImageDialog.open()
    }
    Action {
        text: qsTr("ASCII Art…")
        onTriggered:
            if (isAndroid)
                androidUtils.openTextSaveDialog()
            else {
                var dialog = asciiArtSaveDialog.get()
                dialog.open()
            }
    }
}
