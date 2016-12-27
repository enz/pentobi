import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("&Edit")
    enabled:  ! isRated
    visible: ! isAndroid || enabled

    Menu {
        title: qsTr("Move &Annotation")
        enabled: gameModel.moveNumber > 0
        visible: ! isAndroid || enabled

        ExclusiveGroup { id: moveAnnotationGroup }
        MenuItem {
            text: qsTr("&None")
            checkable: true
            exclusiveGroup: moveAnnotationGroup
            checked: gameModel.moveAnnotation === ""
            onTriggered: gameModel.moveAnnotation = ""
        }
        MenuItem {
            text: qsTr("&Very good")
            checkable: true
            exclusiveGroup: moveAnnotationGroup
            checked: gameModel.moveAnnotation === "!!"
            onTriggered: gameModel.moveAnnotation = "!!"
        }
        MenuItem {
            text: qsTr("&Good")
            checkable: true
            exclusiveGroup: moveAnnotationGroup
            checked: gameModel.moveAnnotation === "!"
            onTriggered: gameModel.moveAnnotation = "!"
        }
        MenuItem {
            text: qsTr("Int&eresting")
            checkable: true
            exclusiveGroup: moveAnnotationGroup
            checked: gameModel.moveAnnotation === "!?"
            onTriggered: gameModel.moveAnnotation = "!?"
        }
        MenuItem {
            text: qsTr("&Doubtful")
            checkable: true
            exclusiveGroup: moveAnnotationGroup
            checked: gameModel.moveAnnotation === "?!"
            onTriggered: gameModel.moveAnnotation = "?!"
        }
        MenuItem {
            text: qsTr("&Bad")
            checkable: true
            exclusiveGroup: moveAnnotationGroup
            checked: gameModel.moveAnnotation === "?"
            onTriggered: gameModel.moveAnnotation = "?"
        }
        MenuItem {
            text: qsTr("Very B&ad")
            checkable: true
            exclusiveGroup: moveAnnotationGroup
            checked: gameModel.moveAnnotation === "??"
            onTriggered: gameModel.moveAnnotation = "??"
        }
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Make &Main Variation")
        enabled: ! gameModel.isMainVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.makeMainVar()
    }
    MenuItem {
        text: qsTr("Move Variation &Up")
        enabled: gameModel.hasPrevVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.moveUpVar()
    }
    MenuItem {
        text: qsTr("Move Variation &Down")
        enabled: gameModel.hasNextVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.moveDownVar()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Delete All Variations")
        enabled: gameModel.hasVariations
        visible: ! isAndroid || enabled
        onTriggered: Logic.deleteAllVar()
    }
    MenuItem {
        text: qsTr("&Truncate")
        enabled: gameModel.canGoBackward
        visible: ! isAndroid || enabled
        onTriggered: Logic.truncate()
    }
    MenuItem {
        text: qsTr("Truncate &Children")
        enabled: gameModel.canGoForward
        visible: ! isAndroid || enabled
        onTriggered: Logic.truncateChildren()
    }
    MenuItem {
        text: qsTr("&Keep Only Position")
        enabled: gameModel.canGoBackward || gameModel.canGoForward
        visible: ! isAndroid || enabled
        onTriggered: Logic.keepOnlyPosition()
    }
    MenuItem {
        text: qsTr("Keep Only &Subtree")
        enabled: gameModel.canGoBackward && gameModel.canGoForward
        visible: ! isAndroid || enabled
        onTriggered: Logic.keepOnlySubtree()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("S&etup Mode")
        checkable: true
        enabled: ! gameModel.canGoBackward && ! gameModel.canGoForward
                 && gameModel.moveNumber === 0
        visible: ! isAndroid || enabled
        checked: gameDisplay.setupMode
        onTriggered: {
            gameDisplay.setupMode = checked
            if (checked) gameDisplay.showPieces()
        }
    }
    MenuItem {
        text: qsTr("&Next Color")
        onTriggered: gameModel.nextColor()
    }
}
