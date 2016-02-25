import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "Main.js" as Logic

RowLayout {
    signal popupMenu()

    spacing: 0

    Item { Layout.preferredWidth: 0.02 * parent.height }
    // App icon
    Image {
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        height: Screen.pixelDensity < 5 ? 24 : 48 // pentobi.svg is 48x48
        width: height
        sourceSize { width: width; height: height }
        source: "icons/pentobi.svg"
        cache: false

    }
    Item { Layout.fillWidth: true }
    AndroidToolButton {
        imageSource: "icons/pentobi-newgame.svg"
        visible: ! gameModel.isGameEmpty
        onClicked: Logic.newGame()
    }
    AndroidToolButton {
        visible: gameModel.canUndo
        imageSource: "icons/pentobi-undo.svg"
        onClicked: Logic.undo()
    }
    AndroidToolButton {
        imageSource: "icons/pentobi-computer-colors.svg"
        onClicked: Logic.showComputerColorDialog()
    }
    AndroidToolButton {
        visible: ! gameModel.isGameOver
        imageSource: "icons/pentobi-play.svg"
        onClicked: Logic.computerPlay()
    }
    AndroidToolButton {
        imageSource: "icons/menu.svg"
        onClicked: popupMenu()
    }
}
