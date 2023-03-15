//-----------------------------------------------------------------------------
/** @file pentobi/qml/ExportImageDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

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
    onAboutToShow: if (! isAndroid) textField.selectAll()
    onAccepted: {
        exportImageWidth = parseInt(textField.text)
        var name = qsTr("Untitled.png")
        if (isAndroid)
            androidUtils.openImageSaveDialog(name)
        else {
            var dialog = imageSaveDialog.get()
            dialog.selectedFile = dialog.currentFolder + "/" + name
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
