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
        Label { text: qsTr("Image width:") }
        TextField {
            id: textField

            text: exportImageWidth
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator{ bottom: 0; top: 32767 }
        }
    }
}
