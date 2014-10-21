import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import "." as Pentobi
import "Main.js" as Logic

ToolBar {
    RowLayout {
        height: parent.height

        Pentobi.ToolButton {
            size: parent.height
            imageSource: "icons/pentobi-newgame.svg"
            onClicked: Logic.newGame(true)
        }
        Pentobi.ToolButton {
            size: parent.height
            imageSource: "icons/pentobi-computer-colors.svg"
            onClicked: Logic.showComputerColorDialog()
        }
        Pentobi.ToolButton {
            size: parent.height
            imageSource: "icons/pentobi-play.svg"
            onClicked: Logic.computerPlay()
        }
    }
}
