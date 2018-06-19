import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    title: qsTr("Export Image")
    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: {
        exportImageWidth = parseInt(textField.text)
        imageSaveDialog.open()
    }

    Column {
        width: Math.min(label.height * 15, 0.95 * rootWindow.width)

        Label {
            id: label

            text: qsTr("Image width:")
        }
        TextField {
            text: exportImageWidth
            width: parent.width
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator{ bottom: 0; top: 32767 }
        }
    }
}
