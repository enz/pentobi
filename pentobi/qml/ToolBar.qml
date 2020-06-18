//-----------------------------------------------------------------------------
/** @file pentobi/qml/ToolBar.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import "." as Pentobi
import "Main.js" as Logic

Item {
    id: root

    // Show toolbar content (menu button is always shown)
    property bool showContent: true

    function clickMenuButton() {
        menuButton.checked = true
        menuButton.onClicked()
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
            elide: Text.ElideRight
            text: Logic.getGameLabel(gameView.setupMode, isRated,
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
            action: actionNew
            visible: showContent && (isDesktop || enabled)
            toolTipText: qsTr("Start a new game")
        }
        Pentobi.Button {
            id: newGameRated

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-rated-game")
            action: actionNewRated
            toolTipText: qsTr("Start a rated game")
        }
        Pentobi.Button {
            id: undo

            icon.source: theme.getImage("pentobi-undo")
            action: actionUndo
            visible: showContent && (isDesktop || enabled)
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    2 * rootWindow.gameView.item.animationDuration : 400
            //: Tooltip for Undo button
            toolTipText: qsTr("Undo move")
        }
        Pentobi.Button {
            id: computerSettings

            icon.source: theme.getImage("pentobi-computer-colors")
            action: actionComputerSettings
            visible: showContent && (isDesktop || enabled)
            toolTipText: qsTr("Set the colors played by the computer")
        }
        Pentobi.Button {
            id: play

            icon.source: theme.getImage("pentobi-play")
            action: actionPlay
            visible: showContent && (isDesktop || enabled)
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    rootWindow.gameView.item.animationDuration : 200
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
        Pentobi.Button {
            id: stop

            icon.source: theme.getImage("pentobi-stop")
            action: actionStop
            visible: showContent && (isDesktop || ! isRated)
            toolTipText: analyzeGameModel.isRunning ?
                              qsTr("Abort game analysis")
                            : qsTr("Abort computer move")
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.3 * parent.height
        }
        Pentobi.Button {
            id: beginning

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-beginning")
            action: actionBeginning
            toolTipText: qsTr("Go to beginning of game")
        }
        Pentobi.Button {
            id: backward10

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-backward10")
            action: actionBackward10
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    rootWindow.gameView.item.animationDuration : 200
            toolTipText: qsTr("Go ten moves backward")
        }
        Pentobi.Button {
            id: backward

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-backward")
            action: actionBackward
            autoRepeat: true
            toolTipText: qsTr("Go one move backward")
        }
        Pentobi.Button {
            id: forward

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-forward")
            action: actionForward
            autoRepeat: true
            toolTipText: qsTr("Go one move forward")
        }
        Pentobi.Button {
            id: forward10

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-forward10")
            action: actionForward10
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    rootWindow.gameView.item.animationDuration : 200
            toolTipText: qsTr("Go ten moves forward")
        }
        Pentobi.Button {
            id: end

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-end")
            action: actionEnd
            toolTipText: qsTr("Go to end of moves")
        }
        Item {
            visible: isDesktop
            Layout.fillWidth: true
            Layout.maximumWidth: 0.3 * parent.height
        }
        Pentobi.Button {
            id: prevVar

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-previous-variation")
            action: actionPrevVar
            autoRepeat: true
            autoRepeatInterval:
                rootWindow.gameView.item ?
                    2 * rootWindow.gameView.item.animationDuration : 400
            toolTipText: qsTr("Go to previous variation")
        }
        Pentobi.Button {
            id: nextVar

            visible: showContent && isDesktop
            icon.source: theme.getImage("pentobi-next-variation")
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
            Layout.maximumWidth: 0.3 * parent.height
        }
        Label {
            visible: showContent && isDesktop
            text: Logic.getGameLabel(gameView.setupMode, isRated,
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
                ToolTip.visible: containsMouse && ! gameView.setupMode
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

            icon.source: theme.getImage(isDesktop ? "menu-desktop" : "menu")
            down: pressed || (isDesktop && menu.item && menu.item.opened)
            onClicked: {
                if (! menu.item)
                    menu.sourceComponent = menuComponent
                if (menu.item.opened)
                    menu.item.close()
                else {
                    gameView.dropCommentFocus()
                    menu.item.popup(0, isDesktop ? height : 0)
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

                    Pentobi.Menu {
                        dynamicWidth: false
                        width: Math.min(font.pixelSize * (isDesktop ? 11 : 18),
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
}
