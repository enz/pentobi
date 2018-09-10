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

Item {
    id: root

    function clickMenuButton() {
        menuButton.checked = true
        menuButton.onClicked()
        menu.item.currentIndex = 0
    }

    // Automatic width in Pentobi.Menu doesn't work with dynamic menus and even
    // if we used a fixed width for the recent files menu, there is a bug
    // (tested with Qt 5.11.1) that makes item texts not vertically centered
    // after insertItem(). So we should call this function to enforce a new
    // menu creation whenever gameModel.recentFiles has changed.
    function destroyMenu() { menu.sourceComponent = null }

    implicitWidth: rowLayout.implicitWidth
    implicitHeight: rowLayout.implicitHeight

    RowLayout {
        id: rowLayout

        anchors.fill: parent
        spacing: 0

        // Like the label used for desktop after the toolbuttons, but with
        // shorter text for small smartphone screens
        Label {
            id: mobileLabel

            visible: ! isDesktop
            color: theme.colorText
            opacity: isRated ? 0.6 : 0.8
            elide: Text.ElideRight
            text: Logic.getGameLabel(gameDisplay.setupMode, isRated,
                                     gameModel.file, gameModel.isModified, true)
            // There is a bug in Qt 5.11 that in some situations elides the
            // text even if there is enough room for it. It doesn't occur if
            // we use implicitWidth + 1 instead if implicitWidth
            Layout.maximumWidth: implicitWidth + 1
            Layout.fillWidth: true
            Layout.leftMargin: root.height / 10

            MouseArea {
                anchors.fill: parent
                onClicked: if (mobileLabel.truncated) ToolTip.visible = true
                ToolTip.text: mobileLabel.text
                ToolTip.timeout: 2000
            }
        }
        Item {
            visible: ! isDesktop
            Layout.fillWidth: true
        }
        Pentobi.Button {
            id: newGame

            icon.source: theme.getImage("pentobi-newgame")
            action: actions.newGame
            visible: isDesktop || enabled
        }
        Pentobi.Button {
            id: newGameRated

            visible: isDesktop
            icon.source: theme.getImage("pentobi-rated-game")
            action: actions.newGameRated
        }
        Pentobi.Button {
            id: undo

            icon.source: theme.getImage("pentobi-undo")
            action: actions.undo
            visible: isDesktop || enabled
        }
        Pentobi.Button {
            id: computerSettings

            icon.source: theme.getImage("pentobi-computer-colors")
            action: actions.computerSettings
            visible: isDesktop || enabled
        }
        Pentobi.Button {
            id: play

            icon.source: theme.getImage("pentobi-play")
            action: actions.play
            visible: isDesktop || enabled
        }
        Pentobi.Button {
            id: stop

            icon.source: theme.getImage("pentobi-stop")
            action: actions.stop
            visible: isDesktop || ! isRated
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.3 * parent.height
        }
        Pentobi.Button {
            id: beginning

            visible: isDesktop
            icon.source: theme.getImage("pentobi-beginning")
            action: actions.beginning
        }
        Pentobi.Button {
            id: backward10

            visible: isDesktop
            icon.source: theme.getImage("pentobi-backward10")
            action: actions.backward10
            autoRepeat: true
            autoRepeatInterval: 750
        }
        Pentobi.Button {
            id: backward

            visible: isDesktop
            icon.source: theme.getImage("pentobi-backward")
            action: actions.backward
            autoRepeat: true
        }
        Pentobi.Button {
            id: forward

            visible: isDesktop
            icon.source: theme.getImage("pentobi-forward")
            action: actions.forward
            autoRepeat: true
        }
        Pentobi.Button {
            id: forward10

            visible: isDesktop
            icon.source: theme.getImage("pentobi-forward10")
            action: actions.forward10
            autoRepeat: true
            autoRepeatInterval: 750
        }
        Pentobi.Button {
            id: end

            visible: isDesktop
            icon.source: theme.getImage("pentobi-end")
            action: actions.end
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.3 * parent.height
        }
        Pentobi.Button {
            id: prevVar

            visible: isDesktop
            icon.source: theme.getImage("pentobi-previous-variation")
            action: actions.prevVar
            autoRepeat: true
            autoRepeatInterval: 750
        }
        Pentobi.Button {
            id: nextVar

            visible: isDesktop
            icon.source: theme.getImage("pentobi-next-variation")
            action: actions.nextVar
            autoRepeat: true
            autoRepeatInterval: 750
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.3 * parent.height
        }
        Label {
            visible: isDesktop
            text: Logic.getGameLabel(gameDisplay.setupMode, isRated,
                                     gameModel.file, gameModel.isModified, false)
            color: theme.colorText
            opacity: 0.8
            elide: Text.ElideRight
            // See comment at Layout.maximumWidth of first label
            Layout.maximumWidth: implicitWidth + 1
            Layout.fillWidth: true

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                ToolTip.text: Logic.getFileInfo(isRated, gameModel.file,
                                                gameModel.isModified)
                ToolTip.visible: containsMouse && ! gameDisplay.setupMode
                                 && (gameModel.file !== "" || isRated)
                ToolTip.delay: 1000
                ToolTip.timeout: 7000
            }
        }
        Item {
            Layout.fillWidth: true
            Layout.maximumWidth: isDesktop ? root.width : 0.3 * parent.height
        }
        Pentobi.Button {
            id: menuButton

            icon.source: theme.getImage("menu")
            down: pressed || (isDesktop && menu.item && menu.item.opened)
            onClicked: {
                if (! menu.item)
                    menu.sourceComponent = menuComponent
                if (menu.item.opened)
                    menu.item.close()
                else {
                    gameDisplay.dropCommentFocus()
                    menu.item.popup(0, isDesktop ? height : 0)
                }
            }

            Loader {
                id: menu

                anchors.fill: parent

                Component {
                    id: menuComponent

                    Pentobi.Menu {
                        dynamicWidth: false
                        width: Math.min(font.pixelSize * (isDesktop ? 11 : 17),
                                        rootWindow.contentItem.width)
                        closePolicy: Popup.CloseOnPressOutsideParent
                                     | Popup.CloseOnEscape

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
        }
    }
    ButtonToolTip {
        button: newGame
        ToolTip.text: qsTr("Start a new game")
    }
    ButtonToolTip {
        button: newGameRated
        ToolTip.text: qsTr("Start a rated game")
    }
    ButtonToolTip {
        button: undo
        //: Tooltip for Undo button
        ToolTip.text: qsTr("Undo move")
    }
    ButtonToolTip {
        button: computerSettings
        ToolTip.text: qsTr("Set the colors played by the computer")
    }
    ButtonToolTip {
        button: play
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
    ButtonToolTip {
        button: stop
        ToolTip.text: analyzeGameModel.isRunning ?
                          qsTr("Abort game analysis")
                        : qsTr("Abort computer move")
    }
    ButtonToolTip {
        button: beginning
        ToolTip.text: qsTr("Go to beginning of game")
    }
    ButtonToolTip {
        button: backward10
        ToolTip.text: qsTr("Go ten moves backward")
    }
    ButtonToolTip {
        button: backward
        ToolTip.text: qsTr("Go one move backward")
    }
    ButtonToolTip {
        button: forward
        ToolTip.text: qsTr("Go one move forward")
    }
    ButtonToolTip {
        button: forward10
        ToolTip.text: qsTr("Go ten moves forward")
    }
    ButtonToolTip {
        button: end
        ToolTip.text: qsTr("Go to end of moves")
    }
    ButtonToolTip {
        button: prevVar
        ToolTip.text: qsTr("Go to previous variation")
    }
    ButtonToolTip {
        button: nextVar
        ToolTip.text: qsTr("Go to next variation")
    }
    ButtonToolTip {
        button: menuButton
    }
}
