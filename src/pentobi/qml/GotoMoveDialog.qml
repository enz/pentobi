import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import "." as Pentobi

Pentobi.Dialog {
    id: root

    title: isDesktop ? qsTr("Go to Move") : ""
    footer: OkCancelButtons { }
    onAccepted: gameModel.gotoMove(parseInt(textField.text))

    Item {
        implicitWidth: {
            var w = rowLayout.implicitWidth
            // Avoid too small width because Default style in Qt 5.11 makes
            // footer no wider than content, which can cause elided text on
            // dialog buttons
            w = Math.max(w, font.pixelSize * 18)
            w = Math.min(w, 0.85 * rootWindow.width)
            return w
        }
        implicitHeight: rowLayout.implicitHeight

        RowLayout {
            id: rowLayout

            anchors.fill: parent

            Label { text: qsTr("Move number:") }
            TextField {
                id: textField

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
                Layout.preferredWidth: font.pixelSize * 5
            }
            Item { Layout.fillWidth: true }
        }
    }
}
