//-----------------------------------------------------------------------------
/** @file pentobi/qml/ExportImageDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2

PentobiDialog {
    id: root

    footer: PentobiDialogButtonBox {
        ButtonOk {
            enabled: textField.acceptableInput
            onClicked: checkAccept()
            DialogButtonBox.buttonRole: DialogButtonBox.InvalidRole
        }
        ButtonCancel { }
    }
    onOpened: if (! isAndroid) textField.selectAll()
    onAccepted: {
        exportImageWidth = parseInt(textField.text)
        var name = gameModel.suggestFileName(folder, "png")
        if (isAndroid)
            androidUtils.openImageSaveDialog(name)
        else {
            var dialog = imageSaveDialog.get()
            dialog.name = name
            dialog.selectNameFilter(0)
            dialog.open()
        }
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
                focus: ! isAndroid
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator{ bottom: 0; top: 32767 }
                selectByMouse: true
                Layout.preferredWidth: font.pixelSize * 5
            }
            Item { Layout.fillWidth: true }
        }
    }
}
