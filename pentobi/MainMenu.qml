//-----------------------------------------------------------------------------
/** @file pentobi/MainMenu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml.Models
import QtQuick.Controls
import "main.js" as Logic

PentobiMenu {
    relativeWidth: 12
    closePolicy: Popup.CloseOnPressOutsideParent | Popup.CloseOnEscape

    PentobiMenu {
        title: qsTr("Game")

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
        PentobiMenu {
            id: root

            title: qsTr("Open Recent")
            relativeWidth: 19
            enabled: instantiator.count > 0

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
                onObjectAdded: (index, obj) => root.insertItem(index, obj)
                onObjectRemoved: (index, obj) => root.removeItem(obj)
            }
            MenuSeparator { }
            MenuItem {
                //: Menu item for clearing the recent files list
                text: qsTr("Clear List")
                // Call recentFiles.clear() after menu is closed because it modifies
                // the menu and otherwise the menu stays visible (Qt 5.15.1)
                onTriggered: Qt.callLater(function() {
                    recentFiles.clear()
                })
            }
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
        MenuSeparator { }
        MenuItem {
            action: actionQuit
        }
    }
    PentobiMenu {
        title: qsTr("Go")

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
    PentobiMenu {
        title: qsTr("Edit")

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
    PentobiMenu {
        title: qsTr("View")

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
    PentobiMenu {
        title: qsTr("Computer")

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
    PentobiMenu {
        title: qsTr("Tools")

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
    PentobiMenu {
        title: qsTr("Help")

        MenuItem {
            action: actionHelp
        }
        MenuItem {
            text: qsTr("About Pentobi")
            onTriggered: aboutDialog.open()
        }
    }
}

