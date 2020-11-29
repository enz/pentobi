//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuEdit.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "Main.js" as Logic

PentobiMenu {
    title: qsTr("Edit")

    PentobiMenuItem {
        text: qsTr("Annotation…")
        enabled: gameModel.moveNumber > 0
        onTriggered: {
            var dialog = moveAnnotationDialog.get()
            dialog.moveNumber = gameModel.moveNumber
            moveAnnotationDialog.open()
        }
    }
    MenuSeparator { }
    PentobiMenuItem {
        text: qsTr("Make Main Variation")
        enabled: ! gameModel.isMainVar && ! isRated
        onTriggered: {
            gameModel.makeMainVar()
            Logic.showTemporaryMessage(qsTr("Made main variation"))
        }
    }
    PentobiMenuItem {
        //: Short for Move Variation Up
        text: qsTr("Variation Up")
        enabled: gameModel.hasPrevVar && ! isRated
        onTriggered: Logic.moveUpVar()
    }
    PentobiMenuItem {
        //: Short for Move Variation Down
        text: qsTr("Variation Down")
        enabled: gameModel.hasNextVar && ! isRated
        onTriggered: Logic.moveDownVar()
    }
    PentobiMenuItem {
        text: qsTr("Delete Variations")
        enabled: gameModel.hasVariations && ! isRated
        onTriggered: Logic.deleteAllVar()
    }
    MenuSeparator { }
    PentobiMenuItem {
        text: qsTr("Truncate")
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: Logic.truncate()
    }
    PentobiMenuItem {
        text: qsTr("Truncate Children")
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: Logic.truncateChildren()
    }
    PentobiMenuItem {
        text: qsTr("Keep Position")
        enabled: ! gameModel.isBoardEmpty && (gameModel.canGoBackward || gameModel.canGoForward) && ! isRated
        onTriggered: Logic.keepOnlyPosition()
    }
    PentobiMenuItem {
        text: qsTr("Keep Subtree")
        enabled: gameModel.canGoBackward && gameModel.canGoForward && ! isRated
        onTriggered: Logic.keepOnlySubtree()
    }
    MenuSeparator { }
    PentobiMenuItem {
        text: qsTr("Setup Mode")
        checkable: true
        enabled: ! gameModel.canGoBackward && ! gameModel.canGoForward
                 && gameModel.moveNumber === 0 && ! isRated
        checked: gameView.setupMode
        onTriggered: {
            checked = ! gameView.setupMode // Workaround for QTBUG-69401
            gameView.setupMode = checked
            if (checked)
                gameView.showPieces()
            else {
                gameView.pickedPiece = null
                Logic.setComputerNone()
            }
        }
    }
    PentobiMenuItem {
        text: qsTr("Next Color")
        enabled: ! isRated
        onTriggered: {
            gameView.pickedPiece = null
            gameModel.nextColor()
        }
    }
}
