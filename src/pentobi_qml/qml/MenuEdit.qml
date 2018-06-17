import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic

Menu {
    title: qsTr("&Edit")
    enabled:  ! isRated

    Menu {
        title: qsTr("Move &Annotation")
        enabled: gameModel.moveNumber > 0

        MenuItem {
            text: qsTr("&None")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === ""
            onTriggered: gameModel.moveAnnotation = ""
        }
        MenuItem {
            text: qsTr("&Very good")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "!!"
            onTriggered: gameModel.moveAnnotation = "!!"
        }
        MenuItem {
            text: qsTr("&Good")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "!"
            onTriggered: gameModel.moveAnnotation = "!"
        }
        MenuItem {
            text: qsTr("Int&eresting")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "!?"
            onTriggered: gameModel.moveAnnotation = "!?"
        }
        MenuItem {
            text: qsTr("&Doubtful")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "?!"
            onTriggered: gameModel.moveAnnotation = "?!"
        }
        MenuItem {
            text: qsTr("&Bad")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "?"
            onTriggered: gameModel.moveAnnotation = "?"
        }
        MenuItem {
            text: qsTr("Very B&ad")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "??"
            onTriggered: gameModel.moveAnnotation = "??"
        }
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Make &Main Variation")
        enabled: ! gameModel.isMainVar
        onTriggered: {
            gameModel.makeMainVar()
            Logic.showTemporaryMessage(qsTr("Made main variation."))
        }
    }
    MenuItem {
        text: qsTr("Move Variation &Up")
        enabled: gameModel.hasPrevVar
        onTriggered: Logic.moveUpVar()
    }
    MenuItem {
        text: qsTr("Move Variation &Down")
        enabled: gameModel.hasNextVar
        onTriggered: Logic.moveDownVar()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Delete All Variations")
        enabled: gameModel.hasVariations
        onTriggered: Logic.deleteAllVar()
    }
    MenuItem {
        text: qsTr("&Truncate")
        enabled: gameModel.canGoBackward
        onTriggered: Logic.truncate()
    }
    MenuItem {
        text: qsTr("Truncate &Children")
        enabled: gameModel.canGoForward
        onTriggered: Logic.truncateChildren()
    }
    MenuItem {
        text: qsTr("&Keep Only Position")
        enabled: ! gameModel.isBoardEmpty && (gameModel.canGoBackward || gameModel.canGoForward)
        onTriggered: Logic.keepOnlyPosition()
    }
    MenuItem {
        text: qsTr("Keep Only &Subtree")
        enabled: gameModel.canGoBackward && gameModel.canGoForward
        onTriggered: Logic.keepOnlySubtree()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("S&etup Mode")
        checkable: true
        enabled: ! gameModel.canGoBackward && ! gameModel.canGoForward
                 && gameModel.moveNumber === 0
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
