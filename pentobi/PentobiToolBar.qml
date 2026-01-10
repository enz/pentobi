//-----------------------------------------------------------------------------
/** @file pentobi/PentobiToolBar.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "main.js" as Logic

Item {
    // Show toolbar content (menu button is always shown)
    property bool showContent: true

    function clickMenuButton() {
        menuButton.clicked()
        menu.item.currentIndex = 0
    }

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

            visible: ! isDesktop && showContent
            color: theme.colorText
            opacity: isRated ? 0.6 : 0.8
            elide: Text.ElideMiddle
            text: Logic.getGameLabel(gameView.setupMode, isRated,
                                     gameModel.file, gameModel.isModified, true)
            Layout.fillWidth: true
            Layout.leftMargin: font.pixelSize / 10

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
        PentobiButton {
            id: newGame

            source: theme.getImage("pentobi-newgame")
            action: actionNew
            visible: showContent && (isDesktop || enabled)
            toolTipText: qsTr("Start a new game")
        }
        PentobiButton {
            id: newGameRated

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-rated-game")
            action: actionNewRated
            toolTipText: qsTr("Start a rated game")
        }
        PentobiButton {
            id: undo

            source: theme.getImage("pentobi-undo")
            action: actionUndo
            visible: showContent && (isDesktop || enabled)
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    2 * rootWindow.gameView.item.animationDuration : 400
            //: Tooltip for Undo button
            toolTipText: qsTr("Undo move")
        }
        PentobiButton {
            id: computerSettings

            source: theme.getImage("pentobi-computer-colors")
            action: actionComputerSettings
            visible: showContent && (isDesktop || enabled)
            toolTipText: qsTr("Set the colors played by the computer")
        }
        PentobiButton {
            id: play

            source: theme.getImage("pentobi-play")
            action: actionPlay
            visible: showContent && (isDesktop || enabled)
            autoRepeat: true
            // Use fast autorepeat to avoid flickering of
            // PentobiButton.pressedAnimation, presses while computer is
            // thinking are ignored anyway.
            autoRepeatInterval: 50
            toolTipText: {
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
        PentobiButton {
            id: stop

            source: theme.getImage("pentobi-stop")
            action: actionStop
            visible: showContent && (isDesktop || ! isRated)
            toolTipText: analyzeGameModel.isRunning ?
                              qsTr("Abort game analysis")
                            : qsTr("Abort computer move")
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.7 * font.pixelSize
        }
        PentobiButton {
            id: beginning

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-beginning")
            action: actionBeginning
            toolTipText: qsTr("Go to beginning of game")
        }
        PentobiButton {
            id: backward10

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-backward10")
            action: actionBackward10
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    rootWindow.gameView.item.animationDuration : 200
            toolTipText: qsTr("Go ten moves backward")
        }
        PentobiButton {
            id: backward

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-backward")
            action: actionBackward
            autoRepeat: true
            toolTipText: qsTr("Go one move backward")
        }
        PentobiButton {
            id: forward

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-forward")
            action: actionForward
            autoRepeat: true
            toolTipText: qsTr("Go one move forward")
        }
        PentobiButton {
            id: forward10

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-forward10")
            action: actionForward10
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    rootWindow.gameView.item.animationDuration : 200
            toolTipText: qsTr("Go ten moves forward")
        }
        PentobiButton {
            id: end

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-end")
            action: actionEnd
            toolTipText: qsTr("Go to end of moves")
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.7 * font.pixelSize
        }
        PentobiButton {
            id: prevVar

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-previous-variation")
            action: actionPrevVar
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    2 * rootWindow.gameView.item.animationDuration : 400
            toolTipText: qsTr("Go to previous variation")
        }
        PentobiButton {
            id: nextVar

            visible: showContent && isDesktop
            source: theme.getImage("pentobi-next-variation")
            action: actionNextVar
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    2 * rootWindow.gameView.item.animationDuration : 400
            toolTipText: qsTr("Go to next variation")
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.7 * font.pixelSize
        }
        Label {
            visible: showContent && isDesktop
            text: Logic.getGameLabel(gameView.setupMode, isRated,
                                     gameModel.file, gameModel.isModified, false)
            color: theme.colorText
            opacity: 0.8
            elide: Text.ElideRight
            Layout.fillWidth: true

            MouseArea {
                anchors { top: parent.top; left: parent.left }
                width: Math.min(parent.implicitWidth, parent.width)
                height: parent.height
                hoverEnabled: true
                ToolTip.text: Logic.getFileInfo(isRated, gameModel.file,
                                                gameModel.isModified)
                ToolTip.visible: containsMouse && ! gameView.setupMode
                                 && (gameModel.file !== "" || isRated)
                ToolTip.delay: 1000
                ToolTip.timeout: 7000
            }
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
        }
        PentobiButton {
            id: menuButton

            source: theme.getImage(isAndroid ? "menu" : "menu-desktop")
            down: isDesktop && (pressed || (menu.item && menu.item.opened))
            onClicked: {
                if (! menu.item)
                    menu.sourceComponent = menuComponent
                if (menu.item.opened)
                    menu.item.close()
                else {
                    gameView.dropCommentFocus()
                    ToolTip.toolTip.hide()
                    menu.item.popup(0, isAndroid ? 0 : height)
                }
            }
            toolTipText: qsTr("Open menu")

            Loader {
                id: menu

                // Having the loader fill the button together with
                // CloseOnPressOutsideParent and the function used in onClicked
                // seems to be the only way to make a click on the button close
                // the menu if it is already open. Is there a better way?
                anchors.fill: parent

                Component {
                    id: menuComponent

                    PentobiMenu {
                        relativeWidth: 12
                        closePolicy: Popup.CloseOnPressOutsideParent
                                     | Popup.CloseOnEscape

                        // QtQuick.Controls.Menu is fixed-width so we need to
                        // test with all supported languages that the items are
                        // not truncated or overlap with shortcuts shown with
                        // newer versions of Qt on KDE (last tested with
                        // Qt 6.7.2)
                        MenuGame { relativeWidth: 29 }
                        MenuGo { relativeWidth: 25 }
                        MenuEdit { relativeWidth: 25 }
                        MenuView { relativeWidth: 25 }
                        MenuComputer { relativeWidth: 25 }
                        MenuTools { relativeWidth: 25 }
                        MenuHelp { relativeWidth: 20 }
                    }
                }
            }
        }
    }
}
