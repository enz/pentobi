//-----------------------------------------------------------------------------
/** @file pentobi/MainMenu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml.Models
import QtQuick
import QtQuick.Controls
import "main.js" as Logic

/** Main menu.
    Reuses Controls.Menu (Popup) and Controls.MenuItem (Item) where possible
    without letting Qt do the submenu positioning to avoid covering the menu
    button if cascade is used. Also adds a navigation header to submenus. */
Item {
    id: root

    property real popupY
    property bool isOpen:
        menuMain.opened
        || menuGame.opened
        || menuRecent.opened
        || menuGo.opened
        || menuEdit.opened
        || menuView.opened
        || menuComputer.opened
        || menuTools.opened
        || menuHelp.opened
    property alias relativeWidth: menuMain.relativeWidth

    function closeMenu() {
        menuMain.close()
        menuGame.close()
        menuRecent.close()
        menuGo.close()
        menuEdit.close()
        menuView.close()
        menuComputer.close()
        menuTools.close()
        menuHelp.close()
    }

    function popupMenu() {
        menuMain.popup(0, popupY)
    }

    component MenuPage: PentobiMenu {
        relativeWidth: root.relativeWidth
        closePolicy: Popup.CloseOnEscape
    }
    component Header: Item {
        property alias text: label.text
        property MenuPage parentMenu

        implicitWidth: Math.max(button.implicitWidth, label.implicitWidth)
        implicitHeight: Math.max(button.implicitHeight, label.implicitHeight)

        PentobiToolButton {
            id: button

            iconSource: "menu-backward"
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            onClicked: if (parentMenu) parentMenu.popup(0, popupY)

        }
        Label {
            id: label

            anchors { centerIn: parent }
            font.bold: true
        }
    }
    component Arrow: Image {

        source: "qrc:/qt/qml/PentobiGui/icons/%1/menu-forward.svg".arg(isDark ? "dark" : "light")
        width: 16
        height: 16
    }
    component SubMenu: MenuItem {
        arrow: Arrow {
            x: parent.width - 1.5 * width
            y: (parent.height - height)/ 2
        }
    }


    MenuPage {
        id: menuMain

        SubMenu {
            text: menuGame.title
            onTriggered: menuGame.popup(0, popupY)
        }
        SubMenu {
            text: menuGo.title
            onTriggered: menuGo.popup(0, popupY)
        }
        SubMenu {
            text: menuEdit.title
            onTriggered: menuEdit.popup(0, popupY)
        }
        SubMenu {
            text: menuView.title
            onTriggered: menuView.popup(0, popupY)
        }
        SubMenu {
            text: menuComputer.title
            onTriggered: menuComputer.popup(0, popupY)
        }
        SubMenu {
            text: menuTools.title
            onTriggered: menuTools.popup(0, popupY)
        }
        SubMenu {
            text: menuHelp.title
            onTriggered: menuHelp.popup(0, popupY)
        }
        MenuSeparator { }
        MenuItem {
            action: actionQuit
        }
    }
    MenuPage {
        id: menuGame

        title: qsTr("Game")

        Header {
            text: menuGame.title
            parentMenu: menuMain
        }
        MenuSeparator { }
        MenuItem {
            action: actionNew
        }
        MenuItem {
            action: actionNewRated
        }
        MenuSeparator { }
        MenuItem {
            text: qsTr("Game Variant…")
            onTriggered: gameVariantDialog.open()
        }
        MenuItem {
            action: actionGameInfo
        }
        MenuSeparator { }
        MenuItem {
            action: actionUndo
        }
        MenuItem {
            action: actionFindMove
        }
        MenuSeparator { }
        MenuItem {
            action: actionOpen
        }
        SubMenu {
            text: menuRecent.title

            enabled: menuRecent.enabled
            onTriggered: menuRecent.popup(0, popupY)
        }
        MenuItem {
            action: actionSave
            enabled: actionSave.enabled && gameModel.file !== ""
        }
        MenuItem {
            action: actionSaveAs
        }
        MenuItem {
            text: qsTr("Export Image…")
            onTriggered: exportImageDialog.open()
        }
    }
    MenuPage {
        id: menuRecent

        title: qsTr("Open Recent")
        enabled: instantiator.count > 0

        Header {
            text: menuRecent.title
            parentMenu: menuGame
        }
        MenuSeparator { }
        Instantiator {
            id: instantiator

            model: recentFiles.entries
            delegate: MenuItem {
                text: {
                    var text = modelData.displayName
                    if (! text || text === "") {
                        text = modelData.file
                        text = text.substring(text.lastIndexOf("/") + 1)
                    }
                    text = text.replace(/\.blksgf$/i, "")
                    return text
                }
                onTriggered:
                    Logic.openRecentFile(modelData.file, modelData.displayName)
            }
            onObjectAdded: (index, obj) => menuRecent.insertItem(index + 2, obj)
            onObjectRemoved: (index, obj) => menuRecent.removeItem(obj)
        }
        MenuSeparator { }
        MenuItem {
            //: Menu item for clearing the recent files list
            text: qsTr("Clear List")
            onTriggered: recentFiles.clear()
        }
    }
    MenuPage {
        id: menuGo

        title: qsTr("Go")

        Header {
            text: menuGo.title
            parentMenu: menuMain
        }
        MenuSeparator { }
        MenuItem {
            action: actionGotoMove
        }
        MenuItem {
            action: actionBackToMainVar
        }
        MenuItem {
            action: actionBeginningOfBranch
        }
        MenuSeparator { }
        MenuItem {
            action: actionNextComment
        }
    }
    MenuPage {
        id: menuEdit

        title: qsTr("Edit")

        Header {
            text: menuEdit.title
            parentMenu: menuMain
        }
        MenuSeparator { }
        MenuItem {
            text: qsTr("Annotation…")
            enabled: gameModel.moveNumber > 0
            onTriggered: {
                var dialog = moveAnnotationDialog.get()
                dialog.moveNumber = gameModel.moveNumber
                moveAnnotationDialog.open()
            }
        }
        MenuSeparator { }
        MenuItem {
            text: qsTr("Make Main Variation")
            enabled: ! gameModel.isMainVar && ! isRated
            onTriggered: {
                gameModel.makeMainVar()
                Logic.showTemporaryMessage(qsTr("Made main variation"))
            }
        }
        MenuItem {
            //: Short for Move Variation Up
            text: qsTr("Variation Up")
            enabled: gameModel.hasPrevVar && ! isRated
            onTriggered: Logic.moveUpVar()
        }
        MenuItem {
            //: Short for Move Variation Down
            text: qsTr("Variation Down")
            enabled: gameModel.hasNextVar && ! isRated
            onTriggered: Logic.moveDownVar()
        }
        MenuItem {
            text: qsTr("Delete Variations")
            enabled: gameModel.hasVariations && ! isRated
            onTriggered: Logic.deleteAllVar()
        }
        MenuSeparator { }
        MenuItem {
            text: qsTr("Truncate")
            enabled: gameModel.canGoBackward && ! isRated
            onTriggered: Logic.truncate()
        }
        MenuItem {
            text: qsTr("Truncate Children")
            enabled: gameModel.canGoForward && ! isRated
            onTriggered: Logic.truncateChildren()
        }
        MenuItem {
            text: qsTr("Keep Position")
            enabled: ! gameModel.isBoardEmpty && (gameModel.canGoBackward || gameModel.canGoForward) && ! isRated
            onTriggered: Logic.keepOnlyPosition()
        }
        MenuItem {
            text: qsTr("Keep Subtree")
            enabled: gameModel.canGoBackward && gameModel.canGoForward && ! isRated
            onTriggered: Logic.keepOnlySubtree()
        }
        MenuSeparator { }
        MenuItem {
            text: qsTr("Setup Mode")
            checkable: true
            enabled: ! gameModel.canGoBackward && ! gameModel.canGoForward
                     && gameModel.moveNumber === 0 && ! isRated
            checked: gameView.setupMode
            onTriggered: {
                gameView.setupMode = checked
                if (checked)
                    gameView.showPieces()
                else {
                    gameView.pickedPiece = null
                    Logic.setComputerNone()
                }
            }
        }
        MenuItem {
            text: qsTr("Next Color")
            enabled: ! isRated
            onTriggered: {
                gameView.pickedPiece = null
                gameModel.nextColor()
            }
        }
    }
    MenuPage {
        id: menuView

        title: qsTr("View")

        Header {
            text: menuView.title
            parentMenu: menuMain
        }
        MenuSeparator { }
        MenuItem {
            text: qsTr("Appearance")
            onTriggered: appearanceDialog.open()
        }
        MenuItem {
            action: actionComment
        }
        MenuItem {
            action: actionFullscreen
        }
    }
    MenuPage {
        id: menuComputer

        title: qsTr("Computer")

        Header {
            text: menuComputer.title
            parentMenu: menuMain
        }
        MenuSeparator { }
        MenuItem {
            action: actionComputerSettings
        }
        MenuItem {
            action: actionPlay
        }
        MenuItem {
            action: actionPlaySingle
        }
        MenuItem {
            action: actionStop
        }
    }
    MenuPage {
        id: menuTools

        title: qsTr("Tools")

        Header {
            text: menuTools.title
            parentMenu: menuMain
        }
        MenuSeparator { }
        MenuItem {
            text: qsTr("Rating")
            onTriggered: Logic.rating()
        }
        MenuItem {
            enabled: ! isRated && ratingModel.numberGames > 0
            text: qsTr("Clear Rating")
            onTriggered: Logic.clearRating()
        }
        MenuSeparator { }
        MenuItem {
            enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
            text: qsTr("Analyze Game…")
            onTriggered: analyzeDialog.open()
        }
        MenuItem {
            enabled: analyzeGameModel.elements.length !== 0
            text: qsTr("Clear Analysis")
            onTriggered: {
                analyzeGameModel.clear()
                gameView.deleteAnalysis()
            }
        }
    }
    MenuPage {
        id: menuHelp

        title: qsTr("Help")

        Header {
            text: menuHelp.title
            parentMenu: menuMain
        }
        MenuSeparator { }
        MenuItem {
            action: actionHelp
        }
        MenuItem {
            text: qsTr("About Pentobi")
            onTriggered: aboutDialog.open()
        }
    }
}
