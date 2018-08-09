import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    title: isAndroid ? "" : qsTr("Go to Move")
    x: (rootWindow.width - width) / 2
    y: (rootWindow.height - height) / 2
    parent: ApplicationWindow.overlay
    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: gameModel.gotoMove(parseInt(textField.text))

    Column {
        width: Math.min(label.height * 15, 0.9 * rootWindow.width)

        Label {
            id: label

            text: qsTr("Move number:")
        }
        TextField {
            id: textField

            width: parent.width
            text: gameModel.moveNumber
            focus: true
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator{
                bottom: 1
                top: gameModel.moveNumber + gameModel.movesLeft
            }
        }
    }
}