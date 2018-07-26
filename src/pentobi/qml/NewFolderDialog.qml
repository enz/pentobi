//-----------------------------------------------------------------------------
/** @file pentobi/qml/NewFolderDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import "." as Pentobi
import "Main.js" as Logic

Pentobi.Dialog {
    id: root

    property url folder

    footer: DialogButtonBox {
        Pentobi.ButtonCancel { }
        Pentobi.ButtonOk { enabled: textField.text.trim().length > 0 }
    }
    onOpened: {
        textField.text = gameModel.suggestNewFolderName(folder)
        textField.selectAll()
    }
    onAccepted: if (! gameModel.createFolder(folder + "/" + textField.text))
                    Logic.showInfo(gameModel.lastInputOutputError)

    Item {
        implicitWidth: {
            // Qt 5.11 doesn't correctly handle dialog sizing if dialog (incl.
            // frame) is wider than window and Default style never makes footer
            // wider than content (potentially eliding button texts).
            var w = rowLayout.implicitWidth
            w = Math.max(w, font.pixelSize * 18)
            w = Math.min(w, 0.9 * rootWindow.width)
            return w
        }
        implicitHeight: rowLayout.implicitHeight

        ColumnLayout {
            id: rowLayout

            anchors.fill: parent

            Label { text: qsTr("Folder name:") }
            TextField {
                id: textField

                focus: true
                selectByMouse: true
                onAccepted: if (textField.text.trim().length > 0) root.accept()
                Layout.fillWidth: true
            }
            Item { Layout.fillWidth: true }
        }
    }
}
