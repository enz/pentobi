//-----------------------------------------------------------------------------
/** @file pentobi/PentobiToolBar.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "main.js" as Logic

ToolBar {
    function clickMenuButton() {
        menuButton.clicked()
        menu.item.currentIndex = 0
    }
    // We don't want the toolbar to be visually separated from the main window
    // content. Also, Material.Dark uses a wrong toolbar color (Qt 6.11).
    // Note that this negatively affects Basic style (toolbutton background
    // doesn't match toolbar)
    background: null

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
        PentobiToolButton {
            iconSource: "pentobi-newgame"
            action: actionNew
            visible: isDesktop || action.enabled
            ToolTip.text: qsTr("Start a new game")
        }
        PentobiToolButton {
            iconSource: "pentobi-rated-game"
            action: actionNewRated
            visible: isDesktop
            ToolTip.text: qsTr("Start a rated game")
        }
        PentobiToolButton {
            iconSource: "pentobi-undo"
            action: actionUndo
            visible: isDesktop || action.enabled
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    2 * rootWindow.gameView.item.animationDuration : 400
            //: Tooltip for Undo button
            ToolTip.text: qsTr("Undo move")
        }
        PentobiToolButton {
            iconSource: "pentobi-computer-colors"
            action: actionComputerSettings
            visible: isDesktop || action.enabled
            ToolTip.text: qsTr("Set the colors played by the computer")
        }
        PentobiToolButton {
            iconSource: "pentobi-play"
            action: actionPlay
            visible: isDesktop || action.enabled
            autoRepeat: true
            // Use fast autorepeat to avoid flickering of
            // PentobiToolButton.pressedAnimation, presses while computer is
            // thinking are ignored anyway.
            autoRepeatInterval: 50
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
        PentobiToolButton {
            iconSource: "pentobi-stop"
            action: actionStop
            visible: isDesktop || ! isRated
            ToolTip.text: analyzeGameModel.isRunning ?
                              qsTr("Abort game analysis")
                            : qsTr("Abort computer move")
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.7 * font.pixelSize
        }
        PentobiToolButton {
            iconSource: "pentobi-beginning"
            action: actionBeginning
            visible: isDesktop
            ToolTip.text: qsTr("Go to beginning of game")
        }
        PentobiToolButton {
            iconSource: "pentobi-backward10"
            action: actionBackward10
            visible: isDesktop
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    rootWindow.gameView.item.animationDuration : 200
            ToolTip.text: qsTr("Go ten moves backward")
        }
        PentobiToolButton {
            iconSource: "pentobi-backward"
            action: actionBackward
            visible: isDesktop
            autoRepeat: true
            ToolTip.text: qsTr("Go one move backward")
        }
        PentobiToolButton {
            iconSource: "pentobi-forward"
            action: actionForward
            visible: isDesktop
            autoRepeat: true
            ToolTip.text: qsTr("Go one move forward")
        }
        PentobiToolButton {
            iconSource: "pentobi-forward10"
            action: actionForward10
            visible: isDesktop
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    rootWindow.gameView.item.animationDuration : 200
            ToolTip.text: qsTr("Go ten moves forward")
        }
        PentobiToolButton {
            iconSource: "pentobi-end"
            action: actionEnd
            visible: isDesktop
            ToolTip.text: qsTr("Go to end of moves")
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.7 * font.pixelSize
        }
        PentobiToolButton {
            iconSource: "pentobi-previous-variation"
            action: actionPrevVar
            visible: isDesktop
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    2 * rootWindow.gameView.item.animationDuration : 400
            ToolTip.text: qsTr("Go to previous variation")
        }
        PentobiToolButton {
            iconSource: "pentobi-next-variation"
            action: actionNextVar
            visible: isDesktop
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    2 * rootWindow.gameView.item.animationDuration : 400
            ToolTip.text: qsTr("Go to next variation")
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.7 * font.pixelSize
        }
        Label {
            visible: isDesktop
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
        PentobiToolButton {
            id: menuButton

            iconSource: "pentobi-menu"
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
            ToolTip.text: qsTr("Open menu")

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
                        // newer versions of Qt
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
