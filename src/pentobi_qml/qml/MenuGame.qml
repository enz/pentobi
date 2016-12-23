import QtQml 2.2
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
    MenuItem {
        text: qsTr("&Rated Game")
        enabled: ! isRated
        visible: ! isAndroid || enabled
        onTriggered: Logic.ratedGame()
    }
    MenuSeparator { }
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
    MenuItem {
        text: qsTr("&Game Info")
        onTriggered: Logic.openGameInfoDialog()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Undo Move")
        enabled: gameModel.canUndo && ! isRated
        visible: ! isAndroid
        onTriggered: Logic.undo()
    }
    MenuItem {
        text: qsTr("&Find Move")
        enabled: ! gameModel.isGameOver
        visible: ! isAndroid || enabled
        onTriggered: Logic.findMove()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Open...")
        onTriggered: Logic.open()
    }
    Menu {
        id: recentFiles

        title: qsTr("Open R&ecent")
        enabled: gameModel.recentFiles.length > 0
        visible: ! isAndroid || enabled

        Instantiator {
            model: gameModel.recentFiles

            MenuItem {
                text: modelData
                onTriggered: Logic.openRecentFile(modelData)
            }
            onObjectAdded: recentFiles.insertItem(index, object)
            onObjectRemoved: recentFiles.removeItem(object)
        }
    }
    MenuItem {
        text: qsTr("&Save")
        enabled: gameModel.file !== "" && gameModel.isModified
        visible: ! isAndroid || enabled
        onTriggered: Logic.save()
    }
    MenuItem {
        text: qsTr("Save &As...")
        enabled: ! gameModel.isGameEmpty
        visible: ! isAndroid || enabled
        onTriggered: Logic.saveAs()
    }
    Menu {
        title: qsTr("&Export")

        MenuItem {
            text: qsTr("&Image")
            onTriggered: exportImageDialog.open()
        }
        MenuItem {
            text: qsTr("&ASCII Art")
            onTriggered: asciiArtSaveDialog.open()
        }
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Quit")
        onTriggered: Qt.quit()
    }
}
