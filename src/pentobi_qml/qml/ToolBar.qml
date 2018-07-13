import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

RowLayout {
    id: root

    property real buttonPadding: Math.round(1.5 * Screen.pixelDensity)

    function clickMenuButton() { menu.currentIndex = 0; menuButton.onClicked() }

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
                else if (gameModel.file !== "")
                    Logic.showInfo(Logic.getFileInfo(gameModel.file, gameModel.isModified))
        }
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-newgame")
        action: actions.actionNew
        visible: ! isAndroid || enabled
        ToolTip.text: qsTr("Start a new game")
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-undo")
        action: actions.actionUndo
        visible: ! isAndroid || enabled
        ToolTip.text: action.text.replace("&", "")
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-computer-colors")
        action: actions.actionComputerColors
        visible: ! isAndroid || enabled
        ToolTip.text: qsTr("Set the colors played by the computer")
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-play")
        action: actions.actionPlay
        visible: ! isAndroid || enabled
        ToolTip.text: {
            var toPlay = gameModel.toPlay
            if (gameModel.gameVariant === "classic_3" && toPlay === 3)
                toPlay = gameModel.altPlayer
            if ((computerPlays0 && toPlay === 0)
                    || (computerPlays1 && toPlay === 1)
                    || (computerPlays2 && toPlay === 2)
                    || (computerPlays3 && toPlay === 3))
                return qsTr("Make the computer continue to play the current color")
            return qsTr("Make the computer play the current color")
        }
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-beginning")
        action: actions.actionBeginning
        ToolTip.text: qsTr("Go to beginning of game")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-backward10")
        action: actions.actionBackward10
        autoRepeat: true
        ToolTip.text: qsTr("Go ten moves backward")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-backward")
        action: actions.actionBackward
        autoRepeat: true
        ToolTip.text: qsTr("Go one move backward")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-forward")
        action: actions.actionForward
        autoRepeat: true
        ToolTip.text: qsTr("Go one move forward")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-forward10")
        action: actions.actionForward10
        autoRepeat: true
        ToolTip.text: qsTr("Go ten moves forward")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-end")
        action: actions.actionEnd
        ToolTip.text: qsTr("Go to end of moves")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-previous-variation")
        action: actions.actionPrevVar
        ToolTip.text: qsTr("Go to previous variation")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-next-variation")
        action: actions.actionNextVar
        ToolTip.text: qsTr("Go to next variation")
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
        color: theme.toolBarTextColor
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            ToolTip.text: Logic.getFileInfo(gameModel.file, gameModel.isModified)
            ToolTip.visible: containsMouse && gameModel.file !== ""
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

        padding: buttonPadding
        imageSource: theme.getImage("menu")
        checkable: true
        down: menu.opened
        onClicked: if (menu.opened) menu.close();
                   else if (isAndroid) menu.popup()
                   else menu.popup(0, height)

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
