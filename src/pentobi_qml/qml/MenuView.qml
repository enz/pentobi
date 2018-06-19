import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Window 2.1
import "Main.js" as Logic

Menu {
    title: Logic.removeShortcut(qsTr("&View"))
    cascade: false

    Menu {
        title: Logic.removeShortcut(qsTr("&Move Marking"))

        MenuItem {
            text: Logic.removeShortcut(qsTr("Last With &Dot"))
            checkable: true
            autoExclusive: true
            checked: gameDisplay.moveMarking !== "last_number"
                     && gameDisplay.moveMarking !== "all_number"
                     && gameDisplay.moveMarking !== "none"
            onTriggered: gameDisplay.moveMarking = "last_dot"
        }
        MenuItem {
            text: Logic.removeShortcut(qsTr("Last With &Number"))
            checkable: true
            autoExclusive: true
            checked: gameDisplay.moveMarking === "last_number"
            onTriggered: gameDisplay.moveMarking = "last_number"
        }
        MenuItem {
            text: Logic.removeShortcut(qsTr("&All With Number"))
            checkable: true
            autoExclusive: true
            checked: gameDisplay.moveMarking === "all_number"
            onTriggered: gameDisplay.moveMarking = "all_number"
        }
        MenuItem {
            text: Logic.removeShortcut(qsTr("N&one"))
            checkable: true
            autoExclusive: true
            checked: gameDisplay.moveMarking === "none"
            onTriggered: gameDisplay.moveMarking = "none"
        }
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Coordinates"))
        checkable: true
        checked: gameDisplay.showCoordinates
        onTriggered: gameDisplay.showCoordinates = checked
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("Show &Variations"))
        checkable: true
        checked: gameModel.showVariations
        onTriggered: gameModel.showVariations = checked
    }
    MenuSeparator { }
    Menu {
        title: Logic.removeShortcut(qsTr("&Theme"))

        MenuItem {
            text: Logic.removeShortcut(qsTr("&Light"))
            checkable: true
            autoExclusive: true
            checked: themeName === "light"
            onTriggered: themeName = "light"
        }
        MenuItem {
            text: Logic.removeShortcut(qsTr("&Dark"))
            checkable: true
            autoExclusive: true
            checked: themeName === "dark"
            onTriggered: themeName = "dark"
        }
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Animate Pieces"))
        checkable: true
        checked: gameDisplay.enableAnimations
        onTriggered: gameDisplay.enableAnimations = checked
    }
    MenuSeparator { }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Fullscreen"))
        checkable: true
        checked: visibility === Window.FullScreen
        onTriggered:
            if (visibility !== Window.FullScreen)
                visibility = Window.FullScreen
            else
                visibility = Window.AutomaticVisibility
    }
}
