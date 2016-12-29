import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls 2.1 as Controls2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

RowLayout {
    id: root

    property alias title: title.text
    property alias titleToolTip: toolTip.text

    function popupMenu() { menu.popup() }

    spacing: 0

    Controls2.Label {
        id: title

        Layout.fillWidth: true
        Layout.leftMargin: root.height / 10
        color: theme.toolBarTextColor
        elide: Text.ElideRight

        Controls2.ToolTip {
            id: toolTip

            timeout: 2500
        }
        MouseArea {
            anchors.fill: parent
            onClicked: toolTip.visible = ! toolTip.visible
        }
    }
    Controls2.Button {
        id: leaveSetup

        visible: gameDisplay.setupMode
        text: qsTr("Leave setup mode")
        Layout.rightMargin: 0.1 * root.height
        background: Rectangle {
            color: leaveSetup.down ? theme.backgroundButtonPressed : "transparent"
            border.color: theme.toolBarTextColor
        }
        contentItem: Controls2.Label {
            text: leaveSetup.text
            color: theme.toolBarTextColor
        }
        onClicked: gameDisplay.setupMode = false
    }
    Pentobi.ToolButton {
        imageSource: "icons/pentobi-newgame.svg"
        visible: ! (gameModel.isGameEmpty && ! isRated) && ! gameDisplay.setupMode
        onClicked: Logic.newGame()
    }
    Pentobi.ToolButton {
        visible: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        imageSource: "icons/pentobi-undo.svg"
        onClicked: Logic.undo()
    }
    Pentobi.ToolButton {
        visible: ! gameDisplay.setupMode
        imageSource: "icons/pentobi-computer-colors.svg"
        onClicked: Logic.showComputerColorDialog()
    }
    Pentobi.ToolButton {
        visible: ! gameModel.isGameOver && ! gameDisplay.setupMode && ! isRated
        imageSource: "icons/pentobi-play.svg"
        onClicked: Logic.computerPlay()
    }
    Pentobi.ToolButton {
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
