import QtQuick 2.0
import QtQuick.Controls 2.3
import "Main.js" as Logic

QtObject {
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
    property Action actionForward: Action {
        shortcut: "Ctrl+Right"
        enabled: gameModel.canGoForward
        onTriggered: gameModel.goForward()
    }
    property Action actionNew: Action {
        shortcut: "Ctrl+N"
        text: Logic.removeShortcut(qsTr("&New"))
        enabled: ! (gameModel.isGameEmpty && gameModel.file === "" && ! gameModel.isModified && ! isRated) && ! gameDisplay.setupMode
        onTriggered: Logic.newGame()
    }
    property Action actionNextVar: Action {
        shortcut: "Ctrl+Down"
        enabled: gameModel.hasNextVar
        onTriggered: gameModel.goNextVar()
    }
    property Action actionPlay: Action {
        shortcut: "Ctrl+L"
        text: Logic.removeShortcut(qsTr("&Play"))
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: Logic.computerPlay()
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
    property Action actionUndo: Action {
        text: Logic.removeShortcut(qsTr("&Undo Move"))
        enabled: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        onTriggered: Logic.undo()
    }
}
