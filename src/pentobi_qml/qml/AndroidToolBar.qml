import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls 2.1 as Controls2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "Main.js" as Logic

RowLayout {
    id: root

    property alias title: title.text

    function popupMenu() { menu.popup() }

    spacing: 0

    Controls2.Label {
        id: title

        Layout.fillWidth: true
        Layout.leftMargin: root.height / 10
        color: theme.androidToolBarTextColor
        elide: Text.ElideRight

        Controls2.ToolTip {
            id: toolTip

            text: title.text
            timeout: 2000
        }
        MouseArea {
            anchors.fill: parent
            onClicked: if (title.truncated) toolTip.open()
        }
    }
    Controls2.Button {
        id: leaveSetup

        visible: gameDisplay.setupMode
        text: qsTr("Leave setup mode")
        background: Rectangle {
            color: leaveSetup.down ? theme.backgroundButtonPressed : "transparent"
            border.color: theme.androidToolBarTextColor
        }
        contentItem: Controls2.Label {
            text: leaveSetup.text
            color: theme.androidToolBarTextColor
        }
        onClicked: gameDisplay.setupMode = false
    }
    AndroidToolButton {
        imageSource: "icons/pentobi-newgame.svg"
        visible: ! (gameModel.isGameEmpty && ! isRated) && ! gameDisplay.setupMode
        onClicked: Logic.newGame()
    }
    AndroidToolButton {
        visible: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        imageSource: "icons/pentobi-undo.svg"
        onClicked: Logic.undo()
    }
    AndroidToolButton {
        visible:  ! gameDisplay.setupMode && ! isRated
        imageSource: "icons/pentobi-computer-colors.svg"
        onClicked: Logic.showComputerColorDialog()
    }
    AndroidToolButton {
        visible: ! gameModel.isGameOver && ! gameDisplay.setupMode && ! isRated
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
        MenuView { }
        MenuComputer { }
        MenuTools { }
        MenuHelp { }
    }
}
