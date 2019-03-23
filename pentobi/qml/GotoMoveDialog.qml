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

    footer: Pentobi.DialogButtonBox {
        ButtonOk {
            enabled: textField.acceptableInput
            onClicked: checkAccept()
            DialogButtonBox.buttonRole: DialogButtonBox.InvalidRole
        }
        ButtonCancel { }
    }
    onOpened: textField.selectAll()
    onAccepted: gameModel.gotoMove(parseInt(textField.text))

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

            Label { text: qsTr("Move number:") }
            TextField {
                id: textField

                text: gameModel.moveNumber === 0 ?
                          gameModel.moveNumber + gameModel.movesLeft : gameModel.moveNumber
                selectByMouse: true
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator{
                    bottom: 0
                    top: gameModel.moveNumber + gameModel.movesLeft
                }
                Layout.preferredWidth: font.pixelSize * 5
                onVisibleChanged: focus = true
            }
            Item { Layout.fillWidth: true }
        }
    }
}
