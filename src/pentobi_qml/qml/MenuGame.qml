import QtQml 2.2
import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: Logic.removeShortcut(qsTr("&Game"))

    MenuItem { action: actions.actionNew }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Rated Game"))
        enabled: ! isRated
        onTriggered: Logic.ratedGame()
    }
    MenuSeparator { }
    Pentobi.Menu {
        title: Logic.removeShortcut(qsTr("Game &Variant"))

        ButtonGroup { id: groupGameVariant }
        Pentobi.Menu {
            title: Logic.removeShortcut(qsTr("&Classic"))

            MenuItemGameVariant {
                gameVariant: "classic_2"
                text: Logic.removeShortcut(qsTr("Classic (&2 Players)"))
            }
            MenuItemGameVariant {
                gameVariant: "classic_3"
                text: Logic.removeShortcut(qsTr("Classic (&3 Players)"))
            }
            MenuItemGameVariant {
                gameVariant: "classic"
                text: Logic.removeShortcut(qsTr("Classic (&4 Players)"))
            }
        }
        MenuItemGameVariant {
            gameVariant: "duo"
            text: Logic.removeShortcut(qsTr("&Duo"))
        }
        MenuItemGameVariant {
            gameVariant: "junior"
            text: Logic.removeShortcut(qsTr("&Junior"))
        }
        Pentobi.Menu {
            title: Logic.removeShortcut(qsTr("&Trigon"))

            MenuItemGameVariant {
                gameVariant: "trigon_2"
                text: Logic.removeShortcut(qsTr("Trigon (&2 Players)"))
            }
            MenuItemGameVariant {
                gameVariant: "trigon_3"
                text: Logic.removeShortcut(qsTr("Trigon (&3 Players)"))
            }
            MenuItemGameVariant {
                gameVariant: "trigon"
                text: Logic.removeShortcut(qsTr("Trigon (&4 Players)"))
            }
        }
        Pentobi.Menu {
            title: Logic.removeShortcut(qsTr("&Nexos"))

            MenuItemGameVariant {
                gameVariant: "nexos_2"
                text: Logic.removeShortcut(qsTr("Nexos (&2 Players)"))
            }
            MenuItemGameVariant {
                gameVariant: "nexos"
                text: Logic.removeShortcut(qsTr("Nexos (&4 Players)"))
            }
        }
        Pentobi.Menu {
            title: Logic.removeShortcut(qsTr("&GembloQ"))

            MenuItemGameVariant {
                gameVariant: "gembloq_2"
                text: Logic.removeShortcut(qsTr("GembloQ (&2 Players, 2 Colors)"))
            }
            MenuItemGameVariant {
                gameVariant: "gembloq_2_4"
                text: Logic.removeShortcut(qsTr("GembloQ (2 &Players, 4 Colors)"))
            }
            MenuItemGameVariant {
                gameVariant: "gembloq_3"
                text: Logic.removeShortcut(qsTr("GembloQ (&3 Players)"))
            }
            MenuItemGameVariant {
                gameVariant: "gembloq"
                text: Logic.removeShortcut(qsTr("GembloQ (&4 Players)"))
            }
        }
        Pentobi.Menu {
            title: Logic.removeShortcut(qsTr("C&allisto"))

            MenuItemGameVariant {
                gameVariant: "callisto_2"
                text: Logic.removeShortcut(qsTr("Callisto (&2 Players, 2 Colors)"))
            }
            MenuItemGameVariant {
                gameVariant: "callisto_2_4"
                text: Logic.removeShortcut(qsTr("Callisto (2 &Players, 4 Colors)"))
            }
            MenuItemGameVariant {
                gameVariant: "callisto_3"
                text: Logic.removeShortcut(qsTr("Callisto (&3 Players)"))
            }
            MenuItemGameVariant {
                gameVariant: "callisto"
                text: Logic.removeShortcut(qsTr("Callisto (&4 Players)"))
            }
        }
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Game Info"))
        onTriggered: Logic.openGameInfoDialog()
    }
    MenuSeparator { }
    MenuItem { action: actions.actionUndo }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Find Move"))
        enabled: ! gameModel.isGameOver
        onTriggered: Logic.findMove()
    }
    MenuSeparator { }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Open..."))
        onTriggered: Logic.open()
    }
    Pentobi.Menu {
        id: recentFiles

        title: Logic.removeShortcut(qsTr("Open R&ecent"))
        enabled: gameModel.recentFiles.length > 0

        Instantiator {
            model: gameModel.recentFiles
            onObjectAdded: recentFiles.insertItem(index, object)
            onObjectRemoved: recentFiles.removeItem(object)

            MenuItem {
                text: modelData.substring(modelData.lastIndexOf("/") + 1)
                onTriggered: Logic.openRecentFile(modelData)
            }
        }
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("Open from &Clipboard"))
        onTriggered: Logic.openClipboard()
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Save"))
        enabled: gameModel.file !== "" && gameModel.isModified
        onTriggered: Logic.save()
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("Save &As..."))
        enabled: ! gameModel.isGameEmpty
        onTriggered: Logic.saveAs()
    }
    Pentobi.Menu {
        title: Logic.removeShortcut(qsTr("&Export"))

        MenuItem {
            text: Logic.removeShortcut(qsTr("&Image"))
            onTriggered: exportImageDialog.open()
        }
        MenuItem {
            text: Logic.removeShortcut(qsTr("&ASCII Art"))
            onTriggered: asciiArtSaveDialog.open()
        }
    }
    MenuSeparator { }
    MenuItem { action: actions.actionQuit }
}
