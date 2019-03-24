//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuEdit.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addShortcut(qsTr("Edit"),
                       //: Keyboard shortcut for menu Edit. Leave empty for no shortcut.
                       qsTr("E"))

    Pentobi.MenuItem {
        text: addShortcut(qsTr("Annotation…"),
                          //: Keyboard shortcut for menu item Annotation. Leave empty for no shortcut.
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
        text: addShortcut(qsTr("Make Main Variation"),
                          //: Keyboard shortcut for menu item Make Main Variation. Leave empty for no shortcut.
                          qsTr("M"))
        enabled: ! gameModel.isMainVar && ! isRated
        onTriggered: {
            gameModel.makeMainVar()
            Logic.showTemporaryMessage(qsTr("Made main variation"))
        }
    }
    Pentobi.MenuItem {
        //: Short for Move Variation Up
        text: addShortcut(qsTr("Variation Up"),
                          //: Keyboard shortcut for menu item Variation Up. Leave empty for no shortcut.
                          qsTr("U"))
        enabled: gameModel.hasPrevVar && ! isRated
        onTriggered: Logic.moveUpVar()
    }
    Pentobi.MenuItem {
        //: Short for Move Variation Down
        text: addShortcut(qsTr("Variation Down"),
                          //: Keyboard shortcut for menu item Variation Down. Leave empty for no shortcut.
                          qsTr("W"))
        enabled: gameModel.hasNextVar && ! isRated
        onTriggered: Logic.moveDownVar()
    }
    Pentobi.MenuItem {
        text: addShortcut(qsTr("Delete Variations"),
                          //: Keyboard shortcut for menu item Delete Variations. Leave empty for no shortcut.
                          qsTr("D"))
        enabled: gameModel.hasVariations && ! isRated
        onTriggered: Logic.deleteAllVar()
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        text: addShortcut(qsTr("Truncate"),
                          //: Keyboard shortcut for menu item Truncate. Leave empty for no shortcut.
                          qsTr("T"))
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: Logic.truncate()
    }
    Pentobi.MenuItem {
        text: addShortcut(qsTr("Truncate Children"),
                          //: Keyboard shortcut for menu item Truncate Children. Leave empty for no shortcut.
                          qsTr("C"))
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: Logic.truncateChildren()
    }
    Pentobi.MenuItem {
        text: addShortcut(qsTr("Keep Position"),
                          //: Keyboard shortcut for menu item Keep Position. Leave empty for no shortcut.
                          qsTr("P"))
        enabled: ! gameModel.isBoardEmpty && (gameModel.canGoBackward || gameModel.canGoForward) && ! isRated
        onTriggered: Logic.keepOnlyPosition()
    }
    Pentobi.MenuItem {
        text: addShortcut(qsTr("Keep Subtree"),
                          //: Keyboard shortcut for menu item Keep Subtree. Leave empty for no shortcut.
                          qsTr("S"))
        enabled: gameModel.canGoBackward && gameModel.canGoForward && ! isRated
        onTriggered: Logic.keepOnlySubtree()
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        text: addShortcut(qsTr("Setup Mode"),
                          //: Keyboard shortcut for menu item Setup Mode. Leave empty for no shortcut.
                          qsTr("O"))
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
    Pentobi.MenuItem {
        text: addShortcut(qsTr("Next Color"),
                          //: Keyboard shortcut for menu item Next Color. Leave empty for no shortcut.
                          qsTr("N"))
        enabled: ! isRated
        onTriggered: {
            gameView.pickedPiece = null
            gameModel.nextColor()
        }
    }
}
