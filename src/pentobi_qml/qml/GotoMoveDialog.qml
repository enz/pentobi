import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.2

Dialog {
    title: qsTr("Go to Move")
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    onAccepted: gameModel.gotoMove(parseInt(textField.text))
    onVisibleChanged: if (! visible) gameDisplay.forceActiveFocus() // QTBUG-48456

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
