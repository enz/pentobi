import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import "Main.js" as Logic

ToolBar {
    RowLayout {
        anchors.fill: parent

        ToolButton {
            iconSource: "icons/pentobi-newgame.svg"
            enabled: ! gameModel.isGameEmpty || isRated
            onClicked: Logic.newGame()
        }
        ToolButton {
            iconSource: "icons/pentobi-undo.svg"
            enabled: gameModel.canUndo && ! isRated
            onClicked: Logic.undo()
        }
        ToolButton {
            iconSource: "icons/pentobi-computer-colors.svg"
            onClicked: Logic.showComputerColorDialog()
        }
        ToolButton {
            iconSource: "icons/pentobi-play.svg"
            enabled: ! isRated
            onClicked: Logic.computerPlay()
        }
        Item { Layout.fillWidth: true }
    }
}
