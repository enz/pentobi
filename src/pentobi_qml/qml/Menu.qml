//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/Menu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

MenuBar {
    /** Don't disable menu items that are not applicable.
        This is currently set to true because it doesn't reliably work on
        Android: if the menu is open, the enabled status will not change until
        the menu is closed and opened again. It is less confusing to the user
        to always enable them even if selecting them does nothing than to
        have inconsistent behavior (last tested with Qt 5.3.2) */
    property bool alwaysEnableAll: true

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
                onTriggered: Logic.changeGameVariant("classic")
            }
            MenuItem {
                text: "Classic (Two Players)"
                checkable: true
                checked: boardModel.gameVariant == "classic_2"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("classic_2")
            }
            MenuItem {
                text: "Duo"
                checkable: true
                checked: boardModel.gameVariant == "duo"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("duo")
            }
            MenuItem {
                text: "Trigon (Four Players)"
                checkable: true
                checked: boardModel.gameVariant == "trigon"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon")
            }
            MenuItem {
                text: "Trigon (Three Players)"
                checkable: true
                checked: boardModel.gameVariant == "trigon_3"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon_3")
            }
            MenuItem {
                text: "Trigon (Two Players)"
                checkable: true
                checked: boardModel.gameVariant == "trigon_2"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon_2")
            }
            MenuItem {
                text: "Junior"
                checkable: true
                checked: boardModel.gameVariant == "junior"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("junior")
            }
        }
        MenuItem {
            text: "Undo Move"
            enabled: alwaysEnableAll || boardModel.canUndo
            onTriggered: Logic.undo()
        }
        MenuItem {
            text: "Move Hint"
            enabled: alwaysEnableAll || ! boardModel.isGameOver
            onTriggered: Logic.moveHint()
        }
        MenuItem {
            text: "Computer Colors"
            onTriggered: Logic.showComputerColorDialog()
        }
        MenuItem {
            text: "Computer Play"
            enabled: alwaysEnableAll || ! playerModel.isGenMoveRunning
            onTriggered: Logic.computerPlay()
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
        Menu {
            title: "Settings"

            Menu {
                title: "Appearance"
                ExclusiveGroup { id: groupTheme }
                MenuItem {
                    text: "Light"
                    checkable: true
                    checked: themeName == "light"
                    exclusiveGroup: groupTheme
                    onTriggered: themeName = "light"
                }
                MenuItem {
                    text: "Dark"
                    checkable: true
                    checked: themeName == "dark"
                    exclusiveGroup: groupTheme
                    onTriggered: themeName = "dark"
                }
            }
            MenuItem {
                text: "Sound"
                checkable: true
                checked: isSoundEnabled
                onTriggered: isSoundEnabled = checked
            }

        }
        MenuItem {
            text: "Quit"
            onTriggered: Qt.quit()
        }
    }
}
