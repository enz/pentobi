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
        visible: ! isDesktop

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
                else if (gameModel.file != "")
                    Logic.showInfo(Logic.getFileInfo(gameModel.file, gameModel.isModified))
        }
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-newgame")
        action: actions.actionNew
        visible: ! isAndroid || enabled
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-undo")
        action: actions.actionUndo
        visible: ! isAndroid || enabled
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-computer-colors")
        action: actions.actionComputerColors
        visible: ! isAndroid || enabled
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-play")
        action: actions.actionPlay
        visible: ! isAndroid || enabled
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-beginning")
        action: actions.actionBeginning
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-backward")
        action: actions.actionBackward
        autoRepeat: true
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-forward")
        action: actions.actionForward
        autoRepeat: true
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-end")
        action: actions.actionEnd
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-previous-variation")
        action: actions.actionPrevVar
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-next-variation")
        action: actions.actionNextVar
    }
    ToolButton {
        visible: isDesktop && gameDisplay.setupMode
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
        visible: isDesktop && (isRated || gameModel.file !== "")
        text: {
            if (isRated) return qsTr("Rated")
            return Logic.getFileLabel(gameModel.file, gameModel.isModified)
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            ToolTip.text: Logic.getFileInfo(gameModel.file, gameModel.isModified)
            ToolTip.visible: containsMouse && gameModel.file != ""
            ToolTip.delay: 700
            ToolTip.timeout: 9000
        }
    }
    Item {
        visible: isDesktop
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
