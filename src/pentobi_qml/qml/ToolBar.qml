import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

ToolBar {
    RowLayout {
        spacing: 0

        // App icon
        Image {
            Layout.leftMargin: 0.05 * width
            Layout.rightMargin: 0.35 * width
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            height: Screen.pixelDensity < 5 ? 24 : 48 // pentobi.svg is 48x48
            width: height
            sourceSize.height: height
            sourceSize.width: width
            source: "icons/pentobi.svg"

        }
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
