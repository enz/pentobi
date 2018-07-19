import QtQml 2.2
import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Window 2.1
import "Main.js" as Logic

QtObject {
    // There are currently several bugs in Qt where a control in a dialog
    // handles a key but does not consume the key event (e.g. QTBUG-69447,
    // QTBUG-69345), so we disable many shortcuts if this property is true,
    // that is when any QtQuickControls2 Popup is open.
    property bool noPopupOpen: overlay.children.length === 0

    property Instantiator actionsPickedNamedPiece: Instantiator {
        model: [ "1", "2", "A", "C", "E", "F", "G", "H", "I", "J", "L",
            "N", "O", "P", "S", "T", "U", "V", "W", "X", "Y", "Z" ]

        Action {
            shortcut: noPopupOpen ? modelData : ""
            onTriggered: Logic.pickNamedPiece(modelData)
        }
    }
    property Action actionBackToMainVar: Action {
        shortcut: "Ctrl+M"
        text: qsTr("Back to Main Variation")
        enabled: ! isRated && ! gameModel.isMainVar
        onTriggered: gameModel.backToMainVar()
    }
    property Action actionBackward: Action {
        shortcut: "Ctrl+Left"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBackward()
    }
    property Action actionBackward10: Action {
        shortcut: "Ctrl+Shift+Left"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBackward10()
    }
    property Action actionBeginning: Action {
        shortcut: "Ctrl+Home"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBeginning()
    }
    property Action actionBeginningOfBranch: Action {
        shortcut: "Ctrl+B"
        text: qsTr("Beginning of Branch")
        enabled: ! isRated && gameModel.hasEarlierVar
        onTriggered: gameModel.gotoBeginningOfBranch()
    }
    property Action actionComputerSettings: Action {
        shortcut: "Ctrl+U"
        text: qsTr("Computer Settings...")
        onTriggered: computerDialog.open()
    }
    property Action actionEnd: Action {
        shortcut: "Ctrl+End"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goEnd()
    }
    property Action actionDropPickedPiece: Action {
        shortcut: noPopupOpen ? "Escape" : ""
        onTriggered: gameDisplay.pickedPiece = null
    }
    property Action actionFindMove: Action {
        shortcut: "F6"
        text: qsTr("Find Move")
        enabled: ! gameModel.isGameOver
        onTriggered: Logic.findMove()
    }
    property Action actionFindNextComment: Action {
        shortcut: "F3"
        text: qsTr("Find Next Comment")
        enabled: ! isRated && (gameModel.canGoForward || gameModel.canGoBackward)
        onTriggered: Logic.findNextComment()
    }
    property Action actionFlipX: Action {
        shortcut: noPopupOpen ? "Alt+Up" : ""
        enabled: gameDisplay.pickedPiece
        onTriggered: gameDisplay.pickedPiece.pieceModel.flipAcrossX()
    }
    property Action actionFlipY: Action {
        shortcut: noPopupOpen ? "Alt+Down" : ""
        enabled: gameDisplay.pickedPiece
        onTriggered: gameDisplay.pickedPiece.pieceModel.flipAcrossY()
    }
    property Action actionForward: Action {
        shortcut: "Ctrl+Right"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goForward()
    }
    property Action actionForward10: Action {
        shortcut: "Ctrl+Shift+Right"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goForward10()
    }
    property Action actionFullscreen: Action {
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
    property Action actionGameInfo: Action {
        shortcut: "Ctrl+I"
        text: qsTr("Game Info")
        onTriggered: gameInfoDialog.open()
    }
    property Action actionGoto: Action {
        shortcut: "Ctrl+G"
        text: qsTr("Go to Move...")
        enabled: ! isRated && (gameModel.moveNumber + gameModel.movesLeft >= 1)
        onTriggered: gotoMoveDialog.open()
    }
    property Action actionHelp: Action {
        shortcut: "F1"
        text: qsTr("Pentobi Help")
        onTriggered: Logic.help()
    }
    property Action actionMovePieceDown: Action {
        shortcut: noPopupOpen ? "Down" : ""
        onTriggered: gameDisplay.shiftPiece(0, 1)
    }
    property Action actionMovePieceDownFast: Action {
        shortcut: noPopupOpen ? "Shift+Down" : ""
        onTriggered: gameDisplay.shiftPieceFast(0, 1)
    }
    property Action actionMovePieceLeft: Action {
        shortcut: noPopupOpen ? "Left" : ""
        onTriggered: gameDisplay.shiftPiece(-1, 0)
    }
    property Action actionMovePieceLeftFast: Action {
        shortcut: noPopupOpen ? "Shift+Left" : ""
        onTriggered: gameDisplay.shiftPieceFast(-1, 0)
    }
    property Action actionMovePieceRight: Action {
        shortcut: noPopupOpen ? "Right" : ""
        onTriggered: gameDisplay.shiftPiece(1, 0)
    }
    property Action actionMovePieceRightFast: Action {
        shortcut: noPopupOpen ? "Shift+Right" : ""
        onTriggered: gameDisplay.shiftPieceFast(1, 0)
    }
    property Action actionMovePieceUp: Action {
        shortcut: noPopupOpen ? "Up" : ""
        onTriggered: gameDisplay.shiftPiece(0, -1)
    }
    property Action actionMovePieceUpFast: Action {
        shortcut: noPopupOpen ? "Shift+Up" : ""
        onTriggered: gameDisplay.shiftPieceFast(0, -1)
    }
    property Action actionNew: Action {
        shortcut: "Ctrl+N"
        text: qsTr("New")
        enabled: gameDisplay.setupMode
                 || ! (gameModel.isGameEmpty && gameModel.file === ""
                       && ! gameModel.isModified && ! isRated)
        onTriggered: Logic.newGame()
    }
    property Action actionNewRated: Action {
        shortcut: "Ctrl+Shift+N"
        text: qsTr("Rated Game")
        enabled: ! isRated
        onTriggered: Logic.ratedGame()
    }
    property Action actionNextOrientation: Action {
        enabled: gameDisplay.pickedPiece
        shortcut: noPopupOpen ? "Space" : ""
        onTriggered: gameDisplay.pickedPiece.pieceModel.nextOrientation()
    }
    property Action actionNextPiece: Action {
        shortcut: noPopupOpen ? "+" : ""
        onTriggered: Logic.nextPiece()
    }
    property Action actionNextVar: Action {
        shortcut: "Ctrl+Down"
        enabled: gameModel.hasNextVar && ! isRated
        onTriggered: gameModel.goNextVar()
    }
    property Action actionOpen: Action {
        shortcut: "Ctrl+O"
        text: qsTr("Open...")
        onTriggered: Logic.open()
    }
    property Action actionPlay: Action {
        shortcut: "Ctrl+L"
        text: qsTr("Play")
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: Logic.computerPlay()
    }
    property Action actionPlaySingle: Action {
        shortcut: "Ctrl+Shift+L"
        text: qsTr("Play Single Move")
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: { isPlaySingleMoveRunning = true; Logic.genMove() }
    }
    property Action actionPrevOrientation: Action {
        enabled: gameDisplay.pickedPiece
        shortcut: noPopupOpen ? "Shift+Space" : ""
        onTriggered: gameDisplay.pickedPiece.pieceModel.previousOrientation()
    }
    property Action actionPrevPiece: Action {
        shortcut: noPopupOpen ? "-" : ""
        onTriggered: Logic.prevPiece()
    }
    property Action actionPrevVar: Action {
        shortcut: "Ctrl+Up"
        enabled: gameModel.hasPrevVar && ! isRated
        onTriggered: gameModel.goPrevVar()
    }
    property Action actionPlayPickedPiece: Action {
        shortcut: noPopupOpen ? "Return" : ""
        onTriggered: gameDisplay.playPickedPiece()
    }
    property Action actionQuit: Action {
        shortcut: "Ctrl+Q"
        text: qsTr("Quit")
        onTriggered: Logic.autoSaveAndQuit()
    }
    property Action actionRating: Action {
        shortcut: "F7"
        text: qsTr("Rating")
        onTriggered: Logic.rating()
    }
    property Action actionRotateLeft: Action {
        shortcut: noPopupOpen ? "Alt+Left" : ""
        enabled: gameDisplay.pickedPiece
        onTriggered: gameDisplay.pickedPiece.pieceModel.rotateLeft()
    }
    property Action actionRotateRight: Action {
        shortcut: noPopupOpen ? "Alt+Right" : ""
        enabled: gameDisplay.pickedPiece
        onTriggered: gameDisplay.pickedPiece.pieceModel.rotateRight()
    }
    property Action actionSave: Action {
        shortcut: "Ctrl+S"
        text: qsTr("Save")
        enabled: ! gameModel.isGameEmpty && gameModel.isModified
        onTriggered: if (gameModel.file !== "") Logic.save(); else Logic.saveAs()
    }
    property Action actionSaveAs: Action {
        shortcut: "Ctrl+Shift+S"
        text: qsTr("Save As...")
        enabled: ! gameModel.isGameEmpty
        onTriggered: Logic.saveAs()
    }
    property Action actionUndo: Action {
        text: qsTr("Undo Move")
        enabled: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        onTriggered: Logic.undo()
    }
}
