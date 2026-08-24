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
    without letting Qt do the SubMenuItem positioning to avoid covering the menu
    button if cascade is used. Also adds a navigation header to SubMenuItems. */
Item {
    id: root

    property real popupY
    property bool isOpen: lastPage && lastPage.opened
    property alias relativeWidth: menuMain.relativeWidth
    property Page lastPage

    function closeMenu() {
        if (lastPage)
            lastPage.close()
    }

    function popupMenu() {
        openPage(menuMain)
    }

    function openPage(page) {
        page.popup(0, popupY)
        lastPage = page
    }

    component Page: PentobiMenu {
        relativeWidth: root.relativeWidth
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    }
    component Header: Item {
        property alias text: label.text
        property Page parentPage

        implicitWidth: Math.max(button.implicitWidth, label.implicitWidth)
        implicitHeight: Math.max(button.implicitHeight, label.implicitHeight)

        PentobiToolButton {
            id: button

            iconSource: "menu-backward"
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            onClicked: if (parentPage) openPage(parentPage)

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
    component SubMenuItem: PentobiMenuItem {
        property Page page

        text: page.title
        arrow: Arrow {
            x: parent.width - 1.5 * width
            y: (parent.height - height)/ 2
            opacity: enabled ? 1 : 0.4
        }
        onTriggered: openPage(page)
    }


    Page {
        id: menuMain

        SubMenuItem {
            page: menuGame
        }
        SubMenuItem {
            page: menuGo
        }
        SubMenuItem {
            page: menuEdit
        }
        SubMenuItem {
            page: menuView
        }
        SubMenuItem {
            page: menuComputer
        }
        SubMenuItem {
            page: menuTools
        }
        SubMenuItem {
            page: menuHelp
        }
        MenuSeparator { }
        PentobiMenuItem {
            action: actionQuit
        }
    }
    Page {
        id: menuGame

        title: qsTr("Game")

        Header {
            text: menuGame.title
            parentPage: menuMain
        }
        MenuSeparator { }
        PentobiMenuItem {
            action: actionNew
        }
        PentobiMenuItem {
            action: actionNewRated
        }
        MenuSeparator { }
        PentobiMenuItem {
            text: qsTr("Game Variant…")
            onTriggered: gameVariantDialog.open()
        }
        PentobiMenuItem {
            action: actionGameInfo
        }
        MenuSeparator { }
        PentobiMenuItem {
            action: actionUndo
        }
        PentobiMenuItem {
            action: actionFindMove
        }
        MenuSeparator { }
        PentobiMenuItem {
            action: actionOpen
        }
        SubMenuItem {
            page: menuRecent
            enabled: menuRecent.enabled
        }
        PentobiMenuItem {
            action: actionSave
            enabled: actionSave.enabled && gameModel.file !== ""
        }
        PentobiMenuItem {
            action: actionSaveAs
        }
        PentobiMenuItem {
            text: qsTr("Export Image…")
            onTriggered: exportImageDialog.open()
        }
    }
    Page {
        id: menuRecent

        title: qsTr("Open Recent")
        enabled: recentFiles.entries.length > 0

        Header {
            text: menuRecent.title
            parentPage: menuGame
        }
        MenuSeparator { }
        Instantiator {
            id: instantiator

            model: recentFiles.entries
            delegate: PentobiMenuItem {
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
        PentobiMenuItem {
            //: Menu item for clearing the recent files list
            text: qsTr("Clear List")
            onTriggered: recentFiles.clear()
        }
    }
    Page {
        id: menuGo

        title: qsTr("Go")

        Header {
            text: menuGo.title
            parentPage: menuMain
        }
        MenuSeparator { }
        PentobiMenuItem {
            action: actionGotoMove
        }
        PentobiMenuItem {
            action: actionBackToMainVar
        }
        PentobiMenuItem {
            action: actionBeginningOfBranch
        }
        MenuSeparator { }
        PentobiMenuItem {
            action: actionNextComment
        }
    }
    Page {
        id: menuEdit

        title: qsTr("Edit")

        Header {
            text: menuEdit.title
            parentPage: menuMain
        }
        MenuSeparator { }
        PentobiMenuItem {
            text: qsTr("Annotation…")
            enabled: gameModel.moveNumber > 0
            onTriggered: {
                var dialog = moveAnnotationDialog.get()
                dialog.moveNumber = gameModel.moveNumber
                moveAnnotationDialog.open()
            }
        }
        MenuSeparator { }
        PentobiMenuItem {
            text: qsTr("Make Main Variation")
            enabled: ! gameModel.isMainVar && ! isRated
            onTriggered: {
                gameModel.makeMainVar()
                Logic.showTemporaryMessage(qsTr("Made main variation"))
            }
        }
        PentobiMenuItem {
            //: Short for Move Variation Up
            text: qsTr("Variation Up")
            enabled: gameModel.hasPrevVar && ! isRated
            onTriggered: Logic.moveUpVar()
        }
        PentobiMenuItem {
            //: Short for Move Variation Down
            text: qsTr("Variation Down")
            enabled: gameModel.hasNextVar && ! isRated
            onTriggered: Logic.moveDownVar()
        }
        PentobiMenuItem {
            text: qsTr("Delete Variations")
            enabled: gameModel.hasVariations && ! isRated
            onTriggered: Logic.deleteAllVar()
        }
        MenuSeparator { }
        PentobiMenuItem {
            text: qsTr("Truncate")
            enabled: gameModel.canGoBackward && ! isRated
            onTriggered: Logic.truncate()
        }
        PentobiMenuItem {
            text: qsTr("Truncate Children")
            enabled: gameModel.canGoForward && ! isRated
            onTriggered: Logic.truncateChildren()
        }
        PentobiMenuItem {
            text: qsTr("Keep Position")
            enabled: ! gameModel.isBoardEmpty && (gameModel.canGoBackward || gameModel.canGoForward) && ! isRated
            onTriggered: Logic.keepOnlyPosition()
        }
        PentobiMenuItem {
            text: qsTr("Keep Subtree")
            enabled: gameModel.canGoBackward && gameModel.canGoForward && ! isRated
            onTriggered: Logic.keepOnlySubtree()
        }
        MenuSeparator { }
        PentobiMenuItem {
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
        PentobiMenuItem {
            text: qsTr("Next Color")
            enabled: ! isRated
            onTriggered: {
                gameView.pickedPiece = null
                gameModel.nextColor()
            }
        }
    }
    Page {
        id: menuView

        title: qsTr("View")

        Header {
            text: menuView.title
            parentPage: menuMain
        }
        MenuSeparator { }
        PentobiMenuItem {
            text: qsTr("Appearance")
            onTriggered: appearanceDialog.open()
        }
        PentobiMenuItem {
            action: actionComment
        }
        PentobiMenuItem {
            action: actionFullscreen
        }
    }
    Page {
        id: menuComputer

        title: qsTr("Computer")

        Header {
            text: menuComputer.title
            parentPage: menuMain
        }
        MenuSeparator { }
        PentobiMenuItem {
            action: actionComputerSettings
        }
        PentobiMenuItem {
            action: actionPlay
        }
        PentobiMenuItem {
            action: actionPlaySingle
        }
        PentobiMenuItem {
            action: actionStop
        }
    }
    Page {
        id: menuTools

        title: qsTr("Tools")

        Header {
            text: menuTools.title
            parentPage: menuMain
        }
        MenuSeparator { }
        PentobiMenuItem {
            text: qsTr("Rating")
            onTriggered: Logic.rating()
        }
        PentobiMenuItem {
            enabled: ! isRated && ratingModel.numberGames > 0
            text: qsTr("Clear Rating")
            onTriggered: Logic.clearRating()
        }
        MenuSeparator { }
        PentobiMenuItem {
            enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
            text: qsTr("Analyze Game…")
            onTriggered: analyzeDialog.open()
        }
        PentobiMenuItem {
            enabled: analyzeGameModel.elements.length !== 0
            text: qsTr("Clear Analysis")
            onTriggered: {
                analyzeGameModel.clear()
                gameView.deleteAnalysis()
            }
        }
    }
    Page {
        id: menuHelp

        title: qsTr("Help")

        Header {
            text: menuHelp.title
            parentPage: menuMain
        }
        MenuSeparator { }
        PentobiMenuItem {
            action: actionHelp
        }
        PentobiMenuItem {
            text: qsTr("About Pentobi")
            onTriggered: aboutDialog.open()
        }
    }
}
