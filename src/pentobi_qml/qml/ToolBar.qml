import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

RowLayout {
    id: root

    property real buttonPadding: Math.round(1.5 * Screen.pixelDensity)

    function openMenu() { menuButton.openMenu() }

    spacing: 0

    Label {
        visible: ! desktopLayout

        text: {
            if (isRated) return qsTr("Rated")
            if (gameDisplay.setupMode) return qsTr("Leave setup")
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
                if (gameDisplay.setupMode) {
                    gameDisplay.setupMode = false
                    Logic.setComputerNone()
                }
                else if (gameModel.file != "") {
                    if (gameModel.isModified)
                        Logic.showInfo(qsTr("File (modified): %1").arg(gameModel.file))
                    else
                        Logic.showInfo(qsTr("File: %1").arg(gameModel.file))
                }
        }
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-newgame")
        enabled: ! (gameModel.isGameEmpty && gameModel.file === "" && ! gameModel.isModified && ! isRated) && ! gameDisplay.setupMode
        visible: ! isAndroid || enabled
        onClicked: Logic.newGame()
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-undo")
        enabled: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        visible: ! isAndroid || enabled
        onClicked: Logic.undo()
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-computer-colors")
        enabled: ! gameDisplay.setupMode
        visible: ! isAndroid || enabled
        onClicked: Logic.showComputerColorDialog()
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-play")
        enabled: ! gameModel.isGameOver && ! gameDisplay.setupMode && ! isRated
        visible: ! isAndroid || enabled
        onClicked: Logic.computerPlay()
    }
    Pentobi.Button {
        visible: desktopLayout
        padding: buttonPadding
        enabled: gameModel.canGoBackward
        imageSource: theme.getImage("pentobi-beginning")
        onClicked: gameModel.goBeginning()
    }
    Pentobi.Button {
        visible: desktopLayout
        padding: buttonPadding
        enabled: gameModel.canGoBackward
        imageSource: theme.getImage("pentobi-backward")
        onClicked: gameModel.goBackward()
        autoRepeat: true
    }
    Pentobi.Button {
        visible: desktopLayout
        padding: buttonPadding
        enabled: gameModel.canGoForward
        imageSource: theme.getImage("pentobi-forward")
        onClicked: gameModel.goForward()
        autoRepeat: true
    }
    Pentobi.Button {
        visible: desktopLayout
        padding: buttonPadding
        enabled: gameModel.canGoForward
        imageSource: theme.getImage("pentobi-end")
        onClicked: gameModel.goEnd()
    }
    Pentobi.Button {
        visible: desktopLayout
        padding: buttonPadding
        enabled: gameModel.hasPrevVar
        imageSource: theme.getImage("pentobi-previous-variation")
        onClicked: gameModel.goPrevVar()
    }
    Pentobi.Button {
        visible: desktopLayout
        padding: buttonPadding
        enabled: gameModel.hasNextVar
        imageSource: theme.getImage("pentobi-next-variation")
        onClicked: gameModel.goNextVar()
    }
    ToolButton {
        visible: desktopLayout && gameDisplay.setupMode
        contentItem: Text {
            color: theme.messageTextColor
            text: qsTr("Leave setup")
        }
        background: Rectangle {
            anchors.fill: parent
            color: parent.pressed ? theme.backgroundButtonPressed : theme.messageBackgroundColor
        }
        onClicked:
            if (gameDisplay.setupMode) {
                gameDisplay.setupMode = false
                Logic.setComputerNone()
            }
    }
    Item { Layout.preferredWidth: parent.height / 2 }
    Label {
        visible: desktopLayout && (isRated || gameModel.file !== "")
        text: {
            if (isRated) return qsTr("Rated")
            return Logic.getFileLabel(gameModel.file, gameModel.isModified)
        }
    }
    Item {
        visible: desktopLayout
        Layout.fillWidth: true
    }
    Pentobi.Button {
        id: menuButton

        function openMenu() { if (isAndroid) menu.popup(); else menu.popup(0, height) }

        padding: buttonPadding
        imageSource: theme.getImage("menu")
        checkable: true
        down: menu.opened
        onClicked: if (menu.opened) menu.close(); else openMenu()

        Pentobi.Menu {
            id: menu

            MenuGame { }
            MenuGo {
                // Note: it's not good enough to set enabled in MenuGo because
                // properties of invisible items are lazily updated (last
                // tested with Qt 5.11)
                enabled: ! isRated && (gameModel.canGoForward || gameModel.canGoBackward)
            }
            MenuEdit {
                // Note: it's not good enough to set enabled in MenuEdit
                // because properties of invisible items are lazily updated
                // (last tested with Qt 5.11)
                enabled: ! isRated
            }
            MenuView { }
            MenuComputer { }
            MenuTools { }
            MenuHelp { }
        }
    }
}
