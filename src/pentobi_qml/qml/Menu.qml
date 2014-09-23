//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/Menu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

MenuBar {
    Menu {
        title: "Game"
        MenuItem {
            text: "New Game"
            onTriggered: Logic.newGame()
        }
        Menu {
            title: "Game Variant"
            ExclusiveGroup { id: groupGameVariant }
            MenuItem {
                text: "Classic (Four Players)"
                checkable: true
                checked: boardModel.gameVariant == "classic"
                exclusiveGroup: groupGameVariant
                onTriggered: changeGameVariantTimer.change("classic")
            }
            MenuItem {
                text: "Classic (Two Players)"
                checkable: true
                checked: boardModel.gameVariant == "classic_2"
                exclusiveGroup: groupGameVariant
                onTriggered: changeGameVariantTimer.change("classic_2")
            }
            MenuItem {
                text: "Duo"
                checkable: true
                checked: boardModel.gameVariant == "duo"
                exclusiveGroup: groupGameVariant
                onTriggered: changeGameVariantTimer.change("duo")
            }
            MenuItem {
                text: "Trigon (Four Players)"
                checkable: true
                checked: boardModel.gameVariant == "trigon"
                exclusiveGroup: groupGameVariant
                onTriggered: changeGameVariantTimer.change("trigon")
            }
            MenuItem {
                text: "Trigon (Three Players)"
                checkable: true
                checked: boardModel.gameVariant == "trigon_3"
                exclusiveGroup: groupGameVariant
                onTriggered: changeGameVariantTimer.change("trigon_3")
            }
            MenuItem {
                text: "Trigon (Two Players)"
                checkable: true
                checked: boardModel.gameVariant == "trigon_2"
                exclusiveGroup: groupGameVariant
                onTriggered: changeGameVariantTimer.change("trigon_2")
            }
            MenuItem {
                text: "Junior"
                checkable: true
                checked: boardModel.gameVariant == "junior"
                exclusiveGroup: groupGameVariant
                onTriggered: changeGameVariantTimer.change("junior")
            }
        }
        MenuItem {
            text: "Undo Move"
            enabled: boardModel.canUndo
            onTriggered: Logic.undo()
        }
        MenuItem {
            text: "Move Hint"
            enabled: ! boardModel.isGameOver
            onTriggered: Logic.moveHint()
        }
        MenuItem {
            text: "Computer Colors"
            onTriggered: Logic.showComputerColorDialog()
        }
        MenuItem {
            text: playerModel.isGenMoveRunning ?
                      "Computer Stop" : "Computer Play"
            onTriggered: playerModel.isGenMoveRunning ?
                             Logic.cancelGenMove() : Logic.computerPlay()
        }
        Menu {
            title: "Level"
            ExclusiveGroup { id: levelGroup }
            MenuItemLevel { level: 1 }
            MenuItemLevel { level: 2 }
            MenuItemLevel { level: 3 }
            MenuItemLevel { level: 4 }
            MenuItemLevel { level: 5 }
            MenuItemLevel { level: 6 }
            MenuItemLevel { level: 7 }
        }
        MenuItem {
            text: "Quit"
            onTriggered: Qt.quit()
        }
    }
}
