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

    footer: DialogButtonBox {
        defaultButton: buttonOk

        ButtonOk { id: buttonOk; enabled: textField.acceptableInput }
        ButtonCancel { }
    }
    onAboutToShow: {
        textField.text = gameModel.moveNumber === 0 ?
                    gameModel.moveNumber + gameModel.movesLeft
                  : gameModel.moveNumber
        if (! isAndroid)
            textField.selectAll()
    }
    onAccepted: gameModel.gotoMove(parseInt(textField.text))

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

                selectByMouse: true
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator{
                    bottom: 0
                    top: gameModel.moveNumber + gameModel.movesLeft
                }
                Layout.preferredWidth: font.pixelSize * 5
                onVisibleChanged: focus = true
                onAccepted: root.accept()
            }
            Item { Layout.fillWidth: true }
        }
    }
}
