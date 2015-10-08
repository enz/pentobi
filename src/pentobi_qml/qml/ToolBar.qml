import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

ToolBar {
    RowLayout {
        anchors.fill: parent

        Pentobi.ToolButton {
            imageSource: "icons/pentobi-newgame.svg"
            onClicked: Logic.newGame(true)
        }
        Pentobi.ToolButton {
            imageSource: "icons/pentobi-undo.svg"
            onClicked: Logic.undo()
        }
        Pentobi.ToolButton {
            imageSource: "icons/pentobi-computer-colors.svg"
            onClicked: Logic.showComputerColorDialog()
        }
        Pentobi.ToolButton {
            imageSource: "icons/pentobi-play.svg"
            onClicked: Logic.computerPlay()
        }
    }
}
