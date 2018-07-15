import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("&Edit")

    Pentobi.Menu {
        title: qsTr("Move &Annotation")
        enabled: gameModel.moveNumber > 0

        Pentobi.MenuItem {
            text: qsTr("&None")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === ""
            onTriggered: gameModel.moveAnnotation = ""
        }
        Pentobi.MenuItem {
            text: qsTr("&Very good")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "!!"
            onTriggered: gameModel.moveAnnotation = "!!"
        }
        Pentobi.MenuItem {
            text: qsTr("&Good")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "!"
            onTriggered: gameModel.moveAnnotation = "!"
        }
        Pentobi.MenuItem {
            text: qsTr("Int&eresting")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "!?"
            onTriggered: gameModel.moveAnnotation = "!?"
        }
        Pentobi.MenuItem {
            text: qsTr("&Doubtful")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "?!"
            onTriggered: gameModel.moveAnnotation = "?!"
        }
        Pentobi.MenuItem {
            text: qsTr("&Bad")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "?"
            onTriggered: gameModel.moveAnnotation = "?"
        }
        Pentobi.MenuItem {
            text: qsTr("Very B&ad")
            checkable: true
            autoExclusive: true
            checked: gameModel.moveAnnotation === "??"
            onTriggered: gameModel.moveAnnotation = "??"
        }
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        text: qsTr("Make &Main Variation")
        enabled: ! gameModel.isMainVar
        onTriggered: {
            gameModel.makeMainVar()
            Logic.showTemporaryMessage(qsTr("Made main variation."))
        }
    }
    Pentobi.MenuItem {
        text: qsTr("Move Variation &Up")
        enabled: gameModel.hasPrevVar
        onTriggered: Logic.moveUpVar()
    }
    Pentobi.MenuItem {
        text: qsTr("Move Variation &Down")
        enabled: gameModel.hasNextVar
        onTriggered: Logic.moveDownVar()
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        text: qsTr("&Delete All Variations")
        enabled: gameModel.hasVariations
        onTriggered: Logic.deleteAllVar()
    }
    Pentobi.MenuItem {
        text: qsTr("&Truncate")
        enabled: gameModel.canGoBackward
        onTriggered: Logic.truncate()
    }
    Pentobi.MenuItem {
        text: qsTr("Truncate &Children")
        enabled: gameModel.canGoForward
        onTriggered: Logic.truncateChildren()
    }
    Pentobi.MenuItem {
        text: qsTr("&Keep Only Position")
        enabled: ! gameModel.isBoardEmpty && (gameModel.canGoBackward || gameModel.canGoForward)
        onTriggered: Logic.keepOnlyPosition()
    }
    Pentobi.MenuItem {
        text: qsTr("Keep Only &Subtree")
        enabled: gameModel.canGoBackward && gameModel.canGoForward
        onTriggered: Logic.keepOnlySubtree()
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        text: qsTr("S&etup Mode")
        checkable: true
        enabled: ! gameModel.canGoBackward && ! gameModel.canGoForward
                 && gameModel.moveNumber === 0
        checked: gameDisplay.setupMode
        onTriggered: {
            // Set checked explicitely because of QTBUG-69401
            checked = ! gameDisplay.setupMode
            gameDisplay.setupMode = checked
            if (checked)
                gameDisplay.showPieces()
            else
                Logic.setComputerNone()
        }
    }
    Pentobi.MenuItem {
        text: qsTr("&Next Color")
        onTriggered: {
            gameDisplay.pickedPiece = null
            gameModel.nextColor()
        }
    }
}
