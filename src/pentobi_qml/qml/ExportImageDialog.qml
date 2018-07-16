import QtQuick 2.0
import QtQuick.Controls 2.2
import "." as Pentobi

Pentobi.Dialog {
    title: isDesktop ? qsTr("Export Image") : ""
    footer: OkCancelButtons { }
    onAccepted: {
        exportImageWidth = parseInt(textField.text)
        imageSaveDialog.open()
    }

    Column {
        width: Math.min(textField.height * 8, 0.9 * rootWindow.width)

        Label { text: qsTr("Image width:") }
        TextField {
            id: textField

            text: exportImageWidth
            width: parent.width
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator{ bottom: 0; top: 32767 }
        }
    }
}
