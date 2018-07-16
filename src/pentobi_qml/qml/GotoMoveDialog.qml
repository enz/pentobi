import QtQuick 2.0
import QtQuick.Controls 2.2
import "." as Pentobi

Pentobi.Dialog {
    id: root

    title: isDesktop ? qsTr("Go to Move") : ""
    footer: OkCancelButtons { }
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
            text: gameModel.moveNumber === 0 ?
                      gameModel.moveNumber + gameModel.movesLeft : gameModel.moveNumber
            focus: true
            inputMethodHints: Qt.ImhDigitsOnly
            validator: IntValidator{
                bottom: 1
                top: gameModel.moveNumber + gameModel.movesLeft
            }
            onVisibleChanged: if (visible) selectAll()
            onAccepted: root.accept()
        }
    }
}
