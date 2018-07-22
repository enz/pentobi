//-----------------------------------------------------------------------------
/** @file pentobi/qml/ToolBar.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

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
        // Like the label used for desktop after the toolbuttons, but with
        // shorter text for small smartphone screens and a message box instead
        // of a tooltip for showing the full file path because tooltips are
        // not wrapped if larger than screen (last tested with Qt 5.11.1)
        visible: ! isDesktop
        Layout.fillWidth: true
        Layout.leftMargin: root.height / 10
        color: theme.colorText
        elide: Text.ElideRight
        text: {
            if (gameDisplay.setupMode) return qsTr("Setup")
            if (isRated) return qsTr("Rated")
            return Logic.getFileLabel(gameModel.file, gameModel.isModified)
        }
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
        icon.source: "icons/pentobi-newgame.svg"
        action: actions.actionNew
        visible: ! isAndroid || enabled
        ToolTip.text: qsTr("Start a new game")
    }
    Pentobi.Button {
        padding: buttonPadding
        icon.source: "icons/pentobi-undo.svg"
        action: actions.actionUndo
        visible: ! isAndroid || enabled
        ToolTip.text: action.text.replace("&", "")
    }
    Pentobi.Button {
        padding: buttonPadding
        icon.source: "icons/pentobi-computer-colors.svg"
        action: actions.actionComputerSettings
        visible: ! isAndroid || enabled
        ToolTip.text: qsTr("Set the colors played by the computer")
    }
    Pentobi.Button {
        padding: buttonPadding
        icon.source: "icons/pentobi-play.svg"
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
        icon.source: "icons/pentobi-beginning.svg"
        action: actions.actionBeginning
        ToolTip.text: qsTr("Go to beginning of game")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        icon.source: "icons/pentobi-backward10.svg"
        action: actions.actionBackward10
        autoRepeat: true
        ToolTip.text: qsTr("Go ten moves backward")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        icon.source: "icons/pentobi-backward.svg"
        action: actions.actionBackward
        autoRepeat: true
        ToolTip.text: qsTr("Go one move backward")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        icon.source: "icons/pentobi-forward.svg"
        action: actions.actionForward
        autoRepeat: true
        ToolTip.text: qsTr("Go one move forward")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        icon.source: "icons/pentobi-forward10.svg"
        action: actions.actionForward10
        autoRepeat: true
        ToolTip.text: qsTr("Go ten moves forward")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        icon.source: "icons/pentobi-end.svg"
        action: actions.actionEnd
        ToolTip.text: qsTr("Go to end of moves")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        icon.source: "icons/pentobi-previous-variation.svg"
        action: actions.actionPrevVar
        ToolTip.text: qsTr("Go to previous variation")
    }
    Pentobi.Button {
        visible: isDesktop
        padding: buttonPadding
        icon.source: "icons/pentobi-next-variation.svg"
        action: actions.actionNextVar
        ToolTip.text: qsTr("Go to next variation")
    }
    Item {
        visible: isDesktop
        Layout.preferredWidth: parent.height / 2
    }
    Label {
        visible: isDesktop && (isRated || gameModel.file !== "" || gameDisplay.setupMode)
        text: {
            if (gameDisplay.setupMode) return qsTr("Setup mode")
            if (isRated) return qsTr("Rated game")
            return Logic.getFileLabel(gameModel.file, gameModel.isModified)
        }
        color: theme.colorText
        elide: Text.ElideRight
        Layout.fillWidth: true

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            ToolTip.text: Logic.getFileInfo(gameModel.file, gameModel.isModified)
            ToolTip.visible: containsMouse && gameModel.file !== "" && ! gameDisplay.setupMode
            ToolTip.delay: 700
            ToolTip.timeout: 9000
        }
    }
    Pentobi.Button {
        id: menuButton

        padding: buttonPadding
        icon.source: "icons/menu.svg"
        checkable: true
        down: menu.opened
        onClicked: if (menu.opened) menu.close();
                   else if (isAndroid) menu.popup()
                   else menu.popup(0, height)

        Pentobi.Menu {
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
}
