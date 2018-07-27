//-----------------------------------------------------------------------------
/** @file pentobi/qml/GotoMoveDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import "." as Pentobi

Pentobi.Dialog {
    id: root

    title: isDesktop ? qsTr("Go to Move") : ""
    footer: DialogButtonBoxOkCancel { }
    onOpened: textField.selectAll()
    onAccepted: gameModel.gotoMove(parseInt(textField.text))

    Item {
        implicitWidth: Math.min(rowLayout.implicitWidth,
                                0.9 * rootWindow.width)
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
                selectByMouse: true
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator{
                    bottom: 0
                    top: gameModel.moveNumber + gameModel.movesLeft
                }
                onAccepted: root.accept()
                Layout.preferredWidth: font.pixelSize * 5
            }
            Item { Layout.fillWidth: true }
        }
    }
}
