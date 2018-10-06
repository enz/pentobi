//-----------------------------------------------------------------------------
/** @file pentobi/qml/Actions.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.2
import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Window 2.1
import "Main.js" as Logic

Item {
    // There are currently several bugs in Qt where a control in a dialog
    // handles a key but does not consume the key event (e.g. QTBUG-69447,
    // QTBUG-69345), so we disable many shortcuts if this property is true,
    // that is when any QtQuickControls2 Popup is open.
    property bool noPopupOpen: rootWindow.Overlay.overlay.children.length === 0

    property Action backToMainVar: Action {
        shortcut: "Ctrl+M"
        text: qsTr("Main Variation")
        enabled: ! isRated && ! gameModel.isMainVar
        onTriggered: Qt.callLater(function() { gameModel.backToMainVar() }) // QTBUG-69682
    }
    property Action backward: Action {
        shortcut: "Ctrl+Left"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBackward()
    }
    property Action backward10: Action {
        shortcut: "Ctrl+Shift+Left"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBackward10()
    }
    property Action beginning: Action {
        shortcut: "Ctrl+Home"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBeginning()
    }
    property Action beginningOfBranch: Action {
        shortcut: "Ctrl+B"
        text: qsTr("Beginning of Branch")
        enabled: ! isRated && gameModel.hasEarlierVar
        onTriggered: Qt.callLater(function() { gameModel.gotoBeginningOfBranch() }) // QTBUG-69682
    }
    property Action comment: Action {
        shortcut: "Ctrl+T"
        text: qsTr("Comment")
        checkable: true
        checked: gameDisplay.isCommentVisible
        onTriggered:
            if (isDesktop)
                gameDisplay.setCommentVisible(checked)
            else {
                if (checked)
                    gameDisplay.showComment()
                else
                    gameDisplay.showPieces()
            }
    }
    property Action computerSettings: Action {
        shortcut: "Ctrl+U"
        //: Menu item Computer/Settings
        text: qsTr("Settings")
        onTriggered: computerDialog.open()
    }
    property Action end: Action {
        shortcut: "Ctrl+End"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goEnd()
    }
    property Action findMove: Action {
        shortcut: "Ctrl+H"
        text: qsTr("Find Move")
        enabled: ! gameModel.isGameOver
        onTriggered: gameDisplay.showMove(gameModel.findMoveNext())
    }
    property Action nextComment: Action {
        shortcut: "Ctrl+E"
        text: qsTr("Next Comment")
        enabled: ! isRated && (gameModel.canGoForward || gameModel.canGoBackward)
        onTriggered: Logic.findNextComment()
    }
    property Action forward: Action {
        shortcut: "Ctrl+Right"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goForward()
    }
    property Action forward10: Action {
        shortcut: "Ctrl+Shift+Right"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goForward10()
    }
    property Action fullscreen: Action {
        shortcut: "F11"
        text: qsTr("Fullscreen")
        checkable: true
        checked: visibility === Window.FullScreen
        onTriggered: {
            if (visibility !== Window.FullScreen)
                visibility = Window.FullScreen
            else
                visibility = Window.AutomaticVisibility
        }
    }
    property Action gameInfo: Action {
        shortcut: "Ctrl+I"
        text: qsTr("Game Info")
        onTriggered: gameInfoDialog.open()
    }
    property Action gotoMove: Action {
        shortcut: "Ctrl+G"
        text: qsTr("Move Number…")
        enabled: ! isRated && (gameModel.moveNumber + gameModel.movesLeft >= 1)
        onTriggered: gotoMoveDialog.open()
    }
    property Action help: Action {
        shortcut: "F1"
        text: qsTr("Pentobi Help")
        onTriggered: Logic.help()
    }
    property Action newGame: Action {
        shortcut: "Ctrl+N"
        text: qsTr("New")
        enabled: gameDisplay.setupMode || gameModel.isModified
                 || gameModel.file !== "" || isRated
        onTriggered: Qt.callLater(function() { Logic.newGame() }) // QTBUG-69682
    }
    property Action newGameRated: Action {
        shortcut: "Ctrl+Shift+N"
        text: qsTr("Rated Game")
        enabled: ! isRated
        onTriggered: Logic.ratedGame()
    }
    property Action nextVar: Action {
        shortcut: "Ctrl+Down"
        enabled: gameModel.hasNextVar && ! isRated
        onTriggered: gameModel.goNextVar()
    }
    property Action open: Action {
        shortcut: "Ctrl+O"
        text: qsTr("Open…")
        onTriggered: Logic.open()
    }
    property Action play: Action {
        shortcut: "Ctrl+L"
        text: qsTr("Play")
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: Logic.computerPlay()
    }
    property Action playSingle: Action {
        shortcut: "Ctrl+Shift+L"
        //: Play a single move
        text: qsTr("Play Move")
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: { isPlaySingleMoveRunning = true; Logic.genMove() }
    }
    property Action prevVar: Action {
        shortcut: "Ctrl+Up"
        enabled: gameModel.hasPrevVar && ! isRated
        onTriggered: gameModel.goPrevVar()
    }
    property Action quit: Action {
        shortcut: "Ctrl+Q"
        text: qsTr("Quit")
        onTriggered: rootWindow.close()
    }
    property Action save: Action {
        shortcut: "Ctrl+S"
        text: qsTr("Save")
        enabled: gameModel.isModified
        onTriggered: if (gameModel.file !== "") Logic.save(); else Logic.saveAs()
    }
    property Action saveAs: Action {
        shortcut: "Ctrl+Shift+S"
        text: qsTr("Save As…")
        enabled: gameModel.isModified || gameModel.file !== ""
        onTriggered: Logic.saveAs()
    }
    property Action stop: Action {
        text: qsTr("Stop")
        enabled: (playerModel.isGenMoveRunning
                  || delayedCheckComputerMove.running
                  || analyzeGameModel.isRunning)
                 && ! isRated
        onTriggered:
            Qt.callLater(function() { Logic.cancelRunning(true) }) // QTBUG-69682
    }
    property Action undo: Action {
        text: qsTr("Undo Move")
        enabled: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        onTriggered: Qt.callLater(function() { Logic.undo() }) // QTBUG-69682
    }

    Instantiator {
        model: [ "1", "2", "A", "C", "E", "F", "G", "H", "I", "J", "L",
            "N", "O", "P", "S", "T", "U", "V", "W", "X", "Y", "Z" ]

        Shortcut {
            sequence: noPopupOpen ? modelData : ""
            onActivated: Logic.pickNamedPiece(modelData)
        }
    }
    Shortcut {
        sequence: isAndroid && noPopupOpen ? "Back" : ""
        onActivated: {
            if (visibility === Window.FullScreen)
                rootWindow.visibility = Window.AutomaticVisibility
            else
                rootWindow.close()
        }
    }
    Shortcut {
        sequence: "Return"
        enabled: ! isAndroid
        onActivated: {
            if (rootWindow.dialogs.length > 0) {
                var dialog = rootWindow.dialogs[rootWindow.dialogs.length - 1]
                dialog.returnPressed()
            }
            else if (noPopupOpen)
                gameDisplay.playPickedPiece()
        }
    }
    Shortcut {
        sequence: noPopupOpen ? "Escape" : ""
        onActivated:
            if (gameDisplay.pickedPiece)
                gameDisplay.pickedPiece = null
            else if (visibility === Window.FullScreen)
                rootWindow.visibility = Window.AutomaticVisibility
    }
    Shortcut {
        sequence: "Ctrl+Shift+H"
        enabled: ! gameModel.isGameOver
        onActivated: gameDisplay.showMove(gameModel.findMovePrevious())
    }
    Shortcut {
        sequence: noPopupOpen ? "Down" : ""
        onActivated: gameDisplay.shiftPiece(0, 1)
    }
    Shortcut {
        sequence: noPopupOpen ? "Shift+Down" : ""
        onActivated: gameDisplay.shiftPieceFast(0, 1)
    }
    Shortcut {
        sequence: noPopupOpen ? "Left" : ""
        onActivated: gameDisplay.shiftPiece(-1, 0)
    }
    Shortcut {
        sequence: noPopupOpen ? "Shift+Left" : ""
        onActivated: gameDisplay.shiftPieceFast(-1, 0)
    }
    Shortcut {
        sequence: noPopupOpen ? "Right" : ""
        onActivated: gameDisplay.shiftPiece(1, 0)
    }
    Shortcut {
        sequence: noPopupOpen ? "Shift+Right" : ""
        onActivated: gameDisplay.shiftPieceFast(1, 0)
    }
    Shortcut {
        sequence: noPopupOpen ? "Up" : ""
        onActivated: gameDisplay.shiftPiece(0, -1)
    }
    Shortcut {
        sequence: noPopupOpen ? "Shift+Up" : ""
        onActivated: gameDisplay.shiftPieceFast(0, -1)
    }
    Shortcut {
        enabled: gameDisplay.pickedPiece
        sequence: noPopupOpen ? "Space" : ""
        onActivated: gameDisplay.pickedPiece.pieceModel.nextOrientation()
    }
    Shortcut {
        sequence: noPopupOpen ? "+" : ""
        onActivated: Logic.nextPiece()
    }
    Shortcut {
        sequence: noPopupOpen ? "Alt+M" : ""
        onActivated: toolBar.clickMenuButton()
    }
    Shortcut {
        enabled: gameDisplay.pickedPiece
        sequence: noPopupOpen ? "Shift+Space" : ""
        onActivated: gameDisplay.pickedPiece.pieceModel.previousOrientation()
    }
    Shortcut {
        sequence: noPopupOpen ? "-" : ""
        onActivated: Logic.prevPiece()
    }
}
