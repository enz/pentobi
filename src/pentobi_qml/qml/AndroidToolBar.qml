import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "Main.js" as Logic

RowLayout {
    function popupMenu() { menu.popup() }

    spacing: 0

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
        imageSource: isAndroid ? "icons/menu.svg" : ""
        menu: menu
    }
    Menu {
        id: menu

        MenuGame { }
        MenuGo { }
        MenuEdit { }
        MenuComputer { }
        MenuView { }
        MenuHelp { }
    }
}
