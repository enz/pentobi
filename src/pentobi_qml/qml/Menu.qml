import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

MenuBar {
    Menu {
        title: qsTr("Game")

        MenuItem {
            text: qsTr("New")
            onTriggered: Logic.newGame(true)
        }
        Menu {
            title: qsTr("Game Variant")

            ExclusiveGroup { id: groupGameVariant }
            MenuItem {
                text: qsTr("Classic (4 Players)")
                checkable: true
                checked: gameModel.gameVariant == "classic"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("classic", true)
            }
            MenuItem {
                text: qsTr("Classic (3 Players)")
                checkable: true
                checked: gameModel.gameVariant == "classic_3"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("classic_3", true)
            }
            MenuItem {
                text: qsTr("Classic (2 Players)")
                checkable: true
                checked: gameModel.gameVariant == "classic_2"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("classic_2", true)
            }
            MenuItem {
                text: qsTr("Duo")
                checkable: true
                checked: gameModel.gameVariant == "duo"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("duo", true)
            }
            MenuItem {
                text: qsTr("Junior")
                checkable: true
                checked: gameModel.gameVariant == "junior"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("junior", true)
            }
            MenuItem {
                text: qsTr("Trigon (4 Players)")
                checkable: true
                checked: gameModel.gameVariant == "trigon"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon", true)
            }
            MenuItem {
                text: qsTr("Trigon (3 Players)")
                checkable: true
                checked: gameModel.gameVariant == "trigon_3"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon_3", true)
            }
            MenuItem {
                text: qsTr("Trigon (2 Players)")
                checkable: true
                checked: gameModel.gameVariant == "trigon_2"
                exclusiveGroup: groupGameVariant
                onTriggered: Logic.changeGameVariant("trigon_2", true)
            }
        }
        MenuItem {
            text: qsTr("Undo Move")
            onTriggered: Logic.undo()
        }
        MenuItem {
            text: qsTr("Find Move")
            onTriggered: Logic.moveHint()
        }
        MenuItem {
            text: qsTr("Quit")
            onTriggered: Qt.quit()
        }
    }
    Menu {
        title: qsTr("Computer")

        MenuItem {
            text: qsTr("Computer Colors")
            onTriggered: Logic.showComputerColorDialog()
        }
        MenuItem {
            text: qsTr("Play")
            onTriggered: Logic.computerPlay()
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
    }

    Menu {
        title: qsTr("View")

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
    }
}
