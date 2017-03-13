import QtQuick 2.0
import QtQuick.Controls 1.1
import Qt.labs.controls 1.0 as Controls2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

RowLayout {
    id: root

    property alias title: title.text

    spacing: 0

    Controls2.Label {
        id: title

        Layout.fillWidth: true
        Layout.leftMargin: root.height / 10
        color: theme.toolBarTextColor
        elide: Text.ElideRight

        //// Tooltip not yet available in Qt.labs.controls
        //Controls2.ToolTip {
        //    id: toolTip
        //
        //    text: title.text
        //    timeout: 2000
        //}
        MouseArea {
            anchors.fill: parent
            onClicked: if (title.truncated) toolTip.open()
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
