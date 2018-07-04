import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Window 2.1
import "Main.js" as Logic

QtObject {
    property Action actionBackToMainVar: Action {
        shortcut: "Ctrl+M"
        text: Logic.removeShortcut(qsTr("Back to &Main Variation"))
        enabled: ! gameModel.isMainVar
        onTriggered: gameModel.backToMainVar()
    }
    property Action actionBackward: Action {
        shortcut: "Ctrl+Left"
        enabled: gameModel.canGoBackward
        onTriggered: gameModel.goBackward()
    }
    property Action actionBeginning: Action {
        shortcut: "Ctrl+Home"
        enabled: gameModel.canGoBackward
        onTriggered: gameModel.goBeginning()
    }
    property Action actionComputerColors: Action {
        shortcut: "Ctrl+U"
        text: Logic.removeShortcut(qsTr("Computer &Colors"))
        onTriggered: Logic.showComputerColorDialog()
    }
    property Action actionEnd: Action {
        shortcut: "Ctrl+End"
        enabled: gameModel.canGoForward
        onTriggered: gameModel.goEnd()
    }
    property Action actionFindMove: Action {
        shortcut: "F6"
        text: Logic.removeShortcut(qsTr("&Find Move"))
        enabled: ! gameModel.isGameOver
        onTriggered: Logic.findMove()
    }
    property Action actionForward: Action {
        shortcut: "Ctrl+Right"
        enabled: gameModel.canGoForward
        onTriggered: gameModel.goForward()
    }
    property Action actionFullscreen: Action {
        shortcut: "F11"
        checkable: true
        checked: visibility === Window.FullScreen
        onTriggered:
            if (visibility !== Window.FullScreen)
                visibility = Window.FullScreen
            else
                visibility = Window.AutomaticVisibility
    }
    property Action actionGameInfo: Action {
        shortcut: "Ctrl+I"
        text: Logic.removeShortcut(qsTr("&Game Info"))
        onTriggered: Logic.openGameInfoDialog()
    }
    property Action actionGoto: Action {
        shortcut: "Ctrl+G"
        text: Logic.removeShortcut(qsTr("&Go to Move..."))
        enabled: gameModel.moveNumber + gameModel.movesLeft > 1
        onTriggered: gotoMoveDialog.open()
    }
    property Action actionHelp: Action {
        shortcut: "F1"
        text: Logic.removeShortcut(qsTr("Pentobi &Help"))
        onTriggered: Logic.help()
    }
    property Action actionNew: Action {
        shortcut: "Ctrl+N"
        text: Logic.removeShortcut(qsTr("&New"))
        enabled: ! (gameModel.isGameEmpty && gameModel.file === ""
                    && ! gameModel.isModified && ! isRated)
                 && ! gameDisplay.setupMode
        onTriggered: Logic.newGame()
    }
    property Action actionNewRated: Action {
        shortcut: "Ctrl+Shift+N"
        text: Logic.removeShortcut(qsTr("&Rated Game"))
        enabled: ! isRated
        onTriggered: Logic.ratedGame()
    }
    property Action actionNextVar: Action {
        shortcut: "Ctrl+Down"
        enabled: gameModel.hasNextVar
        onTriggered: gameModel.goNextVar()
    }
    property Action actionOpen: Action {
        shortcut: "Ctrl+O"
        text: Logic.removeShortcut(qsTr("&Open..."))
        onTriggered: Logic.open()
    }
    property Action actionPlay: Action {
        shortcut: "Ctrl+L"
        text: Logic.removeShortcut(qsTr("&Play"))
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: Logic.computerPlay()
    }
    property Action actionPlaySingle: Action {
        shortcut: "Ctrl+Shift+L"
        text: Logic.removeShortcut(qsTr("Play &Single Move"))
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: { isPlaySingleMoveRunning = true; Logic.genMove() }
    }
    property Action actionPrevVar: Action {
        shortcut: "Ctrl+Up"
        enabled: gameModel.hasPrevVar
        onTriggered: gameModel.goPrevVar()
    }
    property Action actionQuit: Action {
        shortcut: "Ctrl+Q"
        text: Logic.removeShortcut(qsTr("&Quit"))
        onTriggered: Qt.quit()
    }
    property Action actionRating: Action {
        shortcut: "F7"
        text: Logic.removeShortcut(qsTr("&Rating"))
        onTriggered: {
            // Never reuse RatingDialog
            // See comment in Main.qml at ratingModel.onHistoryChanged
            ratingDialog.source = ""
            ratingDialog.open()
        }
    }
    property Action actionSave: Action {
        shortcut: "Ctrl+S"
        text: Logic.removeShortcut(qsTr("&Save"))
        enabled: gameModel.file !== "" && gameModel.isModified
        onTriggered: Logic.save()
    }
    property Action actionSaveAs: Action {
        shortcut: "Ctrl+Shift+S"
        text: Logic.removeShortcut(qsTr("Save &As..."))
        enabled: ! gameModel.isGameEmpty
        onTriggered: Logic.saveAs()
    }
    property Action actionUndo: Action {
        text: Logic.removeShortcut(qsTr("&Undo Move"))
        enabled: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        onTriggered: Logic.undo()
    }
}
