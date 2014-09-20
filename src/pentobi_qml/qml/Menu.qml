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
                onTriggered: Logic.initGameVariant("classic")
            }
            MenuItem {
                text: "Classic (Two Players)"
                checkable: true
                checked: boardModel.gameVariant == "classic_2"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.initGameVariant("classic_2")
            }
            MenuItem {
                text: "Trigon (Four Players)"
                checkable: true
                checked: boardModel.gameVariant == "trigon"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.initGameVariant("trigon")
            }
            MenuItem {
                text: "Trigon (Two Players)"
                checkable: true
                checked: boardModel.gameVariant == "trigon_2"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.initGameVariant("trigon_2")
            }
            MenuItem {
                text: "Duo"
                checkable: true
                checked: boardModel.gameVariant == "duo"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.initGameVariant("duo")
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
            text: "Computer Play"
            enabled: ! playerModel.isGenMoveRunning
            onTriggered: Logic.computerPlay()
        }
        Menu {
            title: "Level"
            ExclusiveGroup { id: levelGroup }
            MenuItem {
                text: "1"
                checkable: true
                checked: playerModel.level == 1
                exclusiveGroup: levelGroup
                onTriggered: playerModel.level = 1
            }
            MenuItem {
                text: "2"
                checkable: true
                checked: playerModel.level == 2
                exclusiveGroup: levelGroup
                onTriggered: playerModel.level = 2
            }
            MenuItem {
                text: "3"
                checkable: true
                checked: playerModel.level == 3
                exclusiveGroup: levelGroup
                onTriggered: playerModel.level = 3
            }
            MenuItem {
                text: "4"
                checkable: true
                checked: playerModel.level == 4
                exclusiveGroup: levelGroup
                onTriggered: playerModel.level = 4
            }
            MenuItem {
                text: "5"
                checkable: true
                checked: playerModel.level == 5
                exclusiveGroup: levelGroup
                onTriggered: playerModel.level = 5
            }
            MenuItem {
                text: "6"
                checkable: true
                checked: playerModel.level == 6
                exclusiveGroup: levelGroup
                onTriggered: playerModel.level = 6
            }
            MenuItem {
                text: "7"
                checkable: true
                checked: playerModel.level == 7
                exclusiveGroup: levelGroup
                onTriggered: playerModel.level = 7
            }
        }
        MenuItem {
            text: "Quit"
            onTriggered: Logic.quit()
        }
    }
}
