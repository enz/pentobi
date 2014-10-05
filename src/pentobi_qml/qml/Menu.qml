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
        title: qsTr("Game")

        MenuItem {
            text: qsTr("New Game")
            onTriggered: Logic.newGame(true)
        }
        MenuItem {
            text: qsTr("Computer Colors")
            onTriggered: Logic.showComputerColorDialog()
        }
        MenuItem {
            text: qsTr("Play")
            enabled: alwaysEnableAll || ! playerModel.isGenMoveRunning
            onTriggered: Logic.computerPlay()
        }
        MenuItem {
            text: qsTr("Undo Move")
            enabled: alwaysEnableAll || boardModel.canUndo
            onTriggered: Logic.undo()
        }
        MenuItem {
            text: qsTr("Move Hint")
            enabled: alwaysEnableAll || ! boardModel.isGameOver
            onTriggered: Logic.moveHint()
        }
        Menu {
            title: qsTr("Game Variant")

            ExclusiveGroup { id: groupGameVariant }
            MenuItem {
                text: qsTr("Classic (4 Players)")
                checkable: true
                checked: boardModel.gameVariant == "classic"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("classic", true)
            }
            MenuItem {
                text: qsTr("Classic (2 Players)")
                checkable: true
                checked: boardModel.gameVariant == "classic_2"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("classic_2", true)
            }
            MenuItem {
                text: qsTr("Duo")
                checkable: true
                checked: boardModel.gameVariant == "duo"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("duo", true)
            }
            MenuItem {
                text: qsTr("Trigon (4 Players)")
                checkable: true
                checked: boardModel.gameVariant == "trigon"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon", true)
            }
            MenuItem {
                text: qsTr("Trigon (3 Players)")
                checkable: true
                checked: boardModel.gameVariant == "trigon_3"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon_3", true)
            }
            MenuItem {
                text: qsTr("Trigon (2 Players)")
                checkable: true
                checked: boardModel.gameVariant == "trigon_2"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon_2", true)
            }
            MenuItem {
                text: qsTr("Junior")
                checkable: true
                checked: boardModel.gameVariant == "junior"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("junior", true)
            }
        }
        Menu {
            title: qsTr("Level")

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
            title: qsTr("Settings")

            MenuItem {
                text: qsTr("Mark Last Move")
                checkable: true
                checked: markLastMove
                onTriggered: {
                    if (markLastMove == checked) return
                    markLastMove = checked
                    Logic.clearMarks()
                    Logic.markLastMove()
                }
            }
            Menu {
                title: qsTr("Appearance")
                ExclusiveGroup { id: groupTheme }
                MenuItem {
                    text: qsTr("Dark")
                    checkable: true
                    checked: themeName == "dark"
                    exclusiveGroup: groupTheme
                    onTriggered: themeName = "dark"
                }
                MenuItem {
                    text: qsTr("Light")
                    checkable: true
                    checked: themeName == "light"
                    exclusiveGroup: groupTheme
                    onTriggered: themeName = "light"
                }
            }
        }
        MenuItem {
            text: qsTr("Quit")
            onTriggered: Qt.quit()
        }
    }
}
