//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuEdit.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Edit"),
                       //: Mnemonic for menu Edit. Leave empty for no mnemonic.
                       qsTr("E"))

    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Annotation…"),
                          //: Mnemonic for menu item Move Annotation. Leave empty for no mnemonic.
                          qsTr("A"))
        enabled: gameModel.moveNumber > 0
        onTriggered: {
            var dialog = moveAnnotationDialog.get()
            dialog.moveNumber = gameModel.moveNumber
            moveAnnotationDialog.open()
        }
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Make Main Variation"),
                          //: Mnemonic for menu item Make Main Variation. Leave empty for no mnemonic.
                          qsTr("M"))
        enabled: ! gameModel.isMainVar && ! isRated
        onTriggered: {
            gameModel.makeMainVar()
            Logic.showTemporaryMessage(qsTr("Made main variation."))
        }
    }
    Pentobi.MenuItem {
        //: Short for Move Variation Up
        text: addMnemonic(qsTr("Variation Up"),
                          //: Mnemonic for menu item Variation Up. Leave empty for no mnemonic.
                          qsTr("U"))
        enabled: gameModel.hasPrevVar && ! isRated
        onTriggered: Logic.moveUpVar()
    }
    Pentobi.MenuItem {
        //: Short for Move Variation Down
        text: addMnemonic(qsTr("Variation Down"),
                          //: Mnemonic for menu item Variation Down. Leave empty for no mnemonic.
                          qsTr("W"))
        enabled: gameModel.hasNextVar && ! isRated
        onTriggered: Logic.moveDownVar()
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Delete Variations"),
                          //: Mnemonic for menu item Delete Variations. Leave empty for no mnemonic.
                          qsTr("D"))
        enabled: gameModel.hasVariations && ! isRated
        onTriggered: Logic.deleteAllVar()
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Truncate"),
                          //: Mnemonic for menu item Truncate. Leave empty for no mnemonic.
                          qsTr("T"))
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: Logic.truncate()
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Truncate Children"),
                          //: Mnemonic for menu item Truncate Children. Leave empty for no mnemonic.
                          qsTr("C"))
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: Logic.truncateChildren()
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Keep Position"),
                          //: Mnemonic for menu item Keep Position. Leave empty for no mnemonic.
                          qsTr("P"))
        enabled: ! gameModel.isBoardEmpty && (gameModel.canGoBackward || gameModel.canGoForward) && ! isRated
        onTriggered: Logic.keepOnlyPosition()
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Keep Subtree"),
                          //: Mnemonic for menu item Keep Subtree. Leave empty for no mnemonic.
                          qsTr("S"))
        enabled: gameModel.canGoBackward && gameModel.canGoForward && ! isRated
        onTriggered: Logic.keepOnlySubtree()
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Setup Mode"),
                          //: Mnemonic for menu item Setup Mode. Leave empty for no mnemonic.
                          qsTr("O"))
        checkable: true
        enabled: ! gameModel.canGoBackward && ! gameModel.canGoForward
                 && gameModel.moveNumber === 0 && ! isRated
        checked: gameDisplay.setupMode
        onTriggered: {
            checked = ! gameDisplay.setupMode // Workaround for QTBUG-69401
            gameDisplay.setupMode = checked
            if (checked)
                gameDisplay.showPieces()
            else
                Logic.setComputerNone()
        }
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Next Color"),
                          //: Mnemonic for menu item Next Color. Leave empty for no mnemonic.
                          qsTr("N"))
        enabled: ! isRated
        onTriggered: {
            gameDisplay.pickedPiece = null
            gameModel.nextColor()
        }
    }
}
