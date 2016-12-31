import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.1

Menu {
    title: qsTr("&View")

    Menu {
        title: qsTr("&Move Marking")

        ExclusiveGroup { id: moveMarkingGroup }
        MenuItem {
            text: qsTr("Last With &Dot")
            checkable: true
            exclusiveGroup: moveMarkingGroup
            checked: gameDisplay.moveMarking !== "last_number"
                     && gameDisplay.moveMarking !== "all_number"
                     && gameDisplay.moveMarking !== "none"
            onTriggered: gameDisplay.moveMarking = "last_dot"
        }
        MenuItem {
            text: qsTr("Last With &Number")
            checkable: true
            exclusiveGroup: moveMarkingGroup
            checked: gameDisplay.moveMarking === "last_number"
            onTriggered: gameDisplay.moveMarking = "last_number"
        }
        MenuItem {
            text: qsTr("&All With Number")
            checkable: true
            exclusiveGroup: moveMarkingGroup
            checked: gameDisplay.moveMarking === "all_number"
            onTriggered: gameDisplay.moveMarking = "all_number"
        }
        MenuItem {
            text: qsTr("N&one")
            checkable: true
            exclusiveGroup: moveMarkingGroup
            checked: gameDisplay.moveMarking === "none"
            onTriggered: gameDisplay.moveMarking = "none"
        }
    }
    MenuItem {
        text: qsTr("&Coordinates")
        checkable: true
        checked: gameDisplay.showCoordinates
        onTriggered: gameDisplay.showCoordinates = checked
    }
    MenuItem {
        text: qsTr("Show &Variations")
        checkable: true
        checked: gameModel.showVariations
        onTriggered: gameModel.showVariations = checked
    }
    MenuItem {
        text: qsTr("&Animate Pieces")
        checkable: true
        checked: gameDisplay.enableAnimations
        onTriggered: gameDisplay.enableAnimations = checked
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Fullscreen")
        checkable: true
        checked: visibility === Window.FullScreen
        onTriggered:
            if (visibility !== Window.FullScreen)
                visibility = Window.FullScreen
            else
                visibility = Window.AutomaticVisibility
    }
}
