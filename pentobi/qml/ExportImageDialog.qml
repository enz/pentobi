//-----------------------------------------------------------------------------
/** @file pentobi/qml/ExportImageDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import "." as Pentobi

Pentobi.Dialog {
    id: root

    footer: Pentobi.DialogButtonBox {
        ButtonOk {
            enabled: textField.acceptableInput
            onClicked: checkAccept()
            DialogButtonBox.buttonRole: DialogButtonBox.InvalidRole
        }
        ButtonCancel { }
    }
    onOpened: textField.selectAll()
    onAccepted: {
        exportImageWidth = parseInt(textField.text)
        var dialog = imageSaveDialog.get()
        dialog.name = gameModel.suggestFileName(folder, "png")
        dialog.selectNameFilter(0)
        dialog.open()
    }

    function returnPressed() {
        if (! hasButtonFocus())
            checkAccept()
    }
    function checkAccept() {
        if (textField.acceptableInput)
            accept()
    }

    Item {
        implicitWidth:
            Math.max(Math.min(rowLayout.implicitWidth, maxContentWidth),
                     minContentWidth)
        implicitHeight: rowLayout.implicitHeight

        RowLayout {
            id: rowLayout

            anchors.fill: parent

            Label { text: qsTr("Image width:") }
            TextField {
                id: textField

                text: exportImageWidth
                focus: true
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator{ bottom: 0; top: 32767 }
                selectByMouse: true
                Layout.preferredWidth: font.pixelSize * 5
            }
            Item { Layout.fillWidth: true }
        }
    }
}
