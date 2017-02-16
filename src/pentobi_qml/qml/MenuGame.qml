import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("&Game")

    MenuItem {
        text: qsTr("&New")
        enabled: ! gameModel.isGameEmpty
        visible: ! isAndroid
        onTriggered: Logic.newGame()
    }
    MenuSeparator {
        visible: ! isAndroid
    }
    Menu {
        title: qsTr("Game &Variant")

        ExclusiveGroup { id: groupGameVariant }
        Menu {
            title: qsTr("&Classic")

            MenuItemGameVariant {
                gameVariant: "classic_2"
                text: qsTr("Classic (&2 Players)")
            }
            MenuItemGameVariant {
                gameVariant: "classic_3"
                text: qsTr("Classic (&3 Players)")
            }
            MenuItemGameVariant {
                gameVariant: "classic"
                text: qsTr("Classic (&4 Players)")
            }
        }
        MenuItemGameVariant {
            gameVariant: "duo"
            text: qsTr("&Duo")
        }
        MenuItemGameVariant {
            gameVariant: "junior"
            text: qsTr("&Junior")
        }
        Menu {
            title: qsTr("&Trigon")

            MenuItemGameVariant {
                gameVariant: "trigon_2"
                text: qsTr("Trigon (&2 Players)")
            }
            MenuItemGameVariant {
                gameVariant: "trigon_3"
                text: qsTr("Trigon (&3 Players)")
            }
            MenuItemGameVariant {
                gameVariant: "trigon"
                text: qsTr("Trigon (&4 Players)")
            }
        }
        Menu {
            title: qsTr("&Nexos")

            MenuItemGameVariant {
                gameVariant: "nexos_2"
                text: qsTr("Nexos (&2 Players)")
            }
            MenuItemGameVariant {
                gameVariant: "nexos"
                text: qsTr("Nexos (&4 Players)")
            }
        }
        Menu {
            title: qsTr("C&allisto")

            MenuItemGameVariant {
                gameVariant: "callisto_2"
                text: qsTr("Callisto (&2 Players)")
            }
            MenuItemGameVariant {
                gameVariant: "callisto_3"
                text: qsTr("Callisto (&3 Players)")
            }
            MenuItemGameVariant {
                gameVariant: "callisto"
                text: qsTr("Callisto (&4 Players)")
            }
        }
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Undo Move")
        enabled: gameModel.canUndo
        visible: ! isAndroid
        onTriggered: Logic.undo()
    }
    MenuItem {
        text: qsTr("&Find Move")
        enabled: ! gameModel.isGameOver
        visible: ! isAndroid || enabled
        onTriggered: Logic.moveHint()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Open...")
        onTriggered: openDialog.open()
    }
    MenuItem {
        text: qsTr("Open from &Clipboard")
        onTriggered: Logic.openFromClipboard()
    }
    MenuItem {
        text: qsTr("&Save As...")
        enabled: ! gameModel.isGameEmpty
        visible: ! isAndroid || enabled
        onTriggered: saveDialog.open()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Quit")
        onTriggered: Qt.quit()
    }
}
