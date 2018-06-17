import QtQuick 2.0
import QtQuick.Controls 2.2

Dialog {
    title: qsTr("Go to Move")
    standardButtons: Dialog.Ok | Dialog.Cancel
    onAccepted: gameModel.gotoMove(parseInt(textField.text))

    Column {
        Label { text: qsTr("Move number:") }
        TextField {
            id: textField

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
