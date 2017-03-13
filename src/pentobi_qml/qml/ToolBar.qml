import QtQuick 2.0
import QtQuick.Controls 1.1
import Qt.labs.controls 1.0 as Controls2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

RowLayout {
    id: root

    spacing: 0

    Controls2.Label {
        text: {
            if (isRated) return qsTr("Rated game")
            if (gameDisplay.setupMode) return qsTr("Leave setup mode")
            if (gameModel.file === "") return ""
            return Logic.getFileLabel(gameModel.file, gameModel.isModified)
        }
        Layout.fillWidth: true
        Layout.leftMargin: root.height / 10
        color: theme.toolBarTextColor
        elide: Text.ElideRight

        MouseArea {
            anchors.fill: parent
            onClicked:
                if (gameDisplay.setupMode)
                    gameDisplay.setupMode = false
                else if (gameModel.file != "") {
                    if (gameModel.isModified)
                        Logic.showInfo(qsTr("File (modified): %1").arg(gameModel.file))
                    else
                        Logic.showInfo(qsTr("File: %1").arg(gameModel.file))
                }
        }
    }
    Pentobi.ToolButton {
        imageSource: theme.getImage("pentobi-newgame")
        enabled: ! (gameModel.isGameEmpty && gameModel.file === "" && ! gameModel.isModified && ! isRated) && ! gameDisplay.setupMode
        onClicked: Logic.newGame()
    }
    Pentobi.ToolButton {
        imageSource: theme.getImage("pentobi-undo")
        enabled: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        onClicked: Logic.undo()
    }
    Pentobi.ToolButton {
        imageSource: theme.getImage("pentobi-computer-colors")
        enabled: ! gameDisplay.setupMode
        onClicked: Logic.showComputerColorDialog()
    }
    Pentobi.ToolButton {
        imageSource: theme.getImage("pentobi-play")
        enabled: ! gameModel.isGameOver && ! gameDisplay.setupMode && ! isRated
        onClicked: Logic.computerPlay()
    }
    Pentobi.ToolButton {
        imageSource: theme.getImage("menu")
        menu: menu
    }
    Menu {
        id: menu

        MenuGame { }
        MenuGo { }
        MenuEdit { }
        MenuView { }
        MenuComputer { }
        MenuTools { }
        MenuHelp { }
    }
}
