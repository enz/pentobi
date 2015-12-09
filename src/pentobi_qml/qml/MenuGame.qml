import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("&Game")

    MenuItem {
        text: qsTr("&New")
        enabled: ! gameModel.isGameEmpty
        visible: ! isAndroid || enabled
        onTriggered: Logic.newGame(true)
    }
    // QTBUG-47847 MenuSeparator { }
    Menu {
        title: qsTr("Game &Variant")

        ExclusiveGroup { id: groupGameVariant }
        MenuItemGameVariant {
            gameVariant: "classic_2"
            text: qsTr("&Classic (2 Players)")
        }
        MenuItemGameVariant {
            gameVariant: "classic_3"
            text: qsTr("Classic (&3 Players)")
        }
        MenuItemGameVariant {
            gameVariant: "classic"
            text: qsTr("Classic (&4 Players)")
        }
        MenuItemGameVariant {
            gameVariant: "duo"
            text: qsTr("&Duo")
        }
        MenuItemGameVariant {
            gameVariant: "junior"
            text: qsTr("&Junior")
        }
        MenuItemGameVariant {
            gameVariant: "trigon_2"
            text: qsTr("&Trigon (2 Players)")
        }
        MenuItemGameVariant {
            gameVariant: "trigon_3"
            text: qsTr("T&rigon (3 Players)")
        }
        MenuItemGameVariant {
            gameVariant: "trigon"
            text: qsTr("Trig&on (4 Players)")
        }
    }
    // QTBUG-47847 MenuSeparator { }
    MenuItem {
        text: qsTr("&Undo Move")
        enabled: gameModel.canUndo
        visible: ! isAndroid || enabled
        onTriggered: Logic.undo()
    }
    MenuItem {
        text: qsTr("&Find Move")
        enabled: ! gameModel.isGameOver
        visible: ! isAndroid || enabled
        onTriggered: Logic.moveHint()
    }
    // QTBUG-47847 MenuSeparator { }
    MenuItem {
        text: qsTr("&Open...")
        onTriggered: Logic.open()
    }
    MenuItem {
        text: qsTr("&Save As...")
        enabled: ! gameModel.isGameEmpty
        visible: ! isAndroid || enabled
        onTriggered: Logic.saveAs()
    }
    // QTBUG-47847 MenuSeparator { }
    MenuItem {
        text: qsTr("&Quit")
        onTriggered: Qt.quit()
    }
}
