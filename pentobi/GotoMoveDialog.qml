//-----------------------------------------------------------------------------
/** @file pentobi/GotoMoveDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

PentobiDialog {
    id: root

    footer: PentobiDialogButtonBox {
        ButtonOk {
            enabled: textField.acceptableInput
            onClicked: checkAccept()
            DialogButtonBox.buttonRole: DialogButtonBox.InvalidRole
        }
        ButtonCancel { }
    }
    onAboutToShow: textField.text = ""
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

                placeholderText:
                    gameModel.moveNumber === 0 ?
                        gameModel.moveNumber + gameModel.movesLeft
                      : gameModel.moveNumber
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
