import QtQml 2.2
import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("&Game")

    Pentobi.MenuItem { action: actions.actionNew }
    Pentobi.MenuItem { action: actions.actionNewRated }
    MenuSeparator { }
    Pentobi.Menu {
        title: qsTr("Game &Variant")

        ButtonGroup { id: groupGameVariant }
        Pentobi.Menu {
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
        Pentobi.Menu {
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
        Pentobi.Menu {
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
        Pentobi.Menu {
            title: qsTr("&GembloQ")

            MenuItemGameVariant {
                gameVariant: "gembloq_2"
                text: qsTr("GembloQ (&2 Players, 2 Colors)")
            }
            MenuItemGameVariant {
                gameVariant: "gembloq_2_4"
                text: qsTr("GembloQ (2 &Players, 4 Colors)")
            }
            MenuItemGameVariant {
                gameVariant: "gembloq_3"
                text: qsTr("GembloQ (&3 Players)")
            }
            MenuItemGameVariant {
                gameVariant: "gembloq"
                text: qsTr("GembloQ (&4 Players)")
            }
        }
        Pentobi.Menu {
            title: qsTr("C&allisto")

            MenuItemGameVariant {
                gameVariant: "callisto_2"
                text: qsTr("Callisto (&2 Players, 2 Colors)")
            }
            MenuItemGameVariant {
                gameVariant: "callisto_2_4"
                text: qsTr("Callisto (2 &Players, 4 Colors)")
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
    Pentobi.MenuItem { action: actions.actionGameInfo }
    MenuSeparator { }
    Pentobi.MenuItem { action: actions.actionUndo }
    Pentobi.MenuItem { action: actions.actionFindMove }
    MenuSeparator { }
    Pentobi.MenuItem { action: actions.actionOpen }
    Pentobi.Menu {
        id: recentFiles

        title: qsTr("Open R&ecent")
        enabled: gameModel.recentFiles.length > 0

        Instantiator {
            model: gameModel.recentFiles
            onObjectAdded: recentFiles.insertItem(index, object)
            onObjectRemoved: recentFiles.removeItem(object)

            Pentobi.MenuItem {
                text: {
                    var result = modelData.substring(modelData.lastIndexOf("/") + 1)
                    if (! isAndroid)
                        result = "&" + (index + 1) + ". " + result
                    return result
                }
                onTriggered: Logic.openRecentFile(modelData)
            }
        }
    }
    Pentobi.MenuItem {
        text: qsTr("Open from &Clipboard")
        onTriggered: Logic.openClipboard()
    }
    Pentobi.MenuItem { action: actions.actionSave }
    Pentobi.MenuItem { action: actions.actionSaveAs }
    Pentobi.Menu {
        title: qsTr("&Export")

        Pentobi.MenuItem {
            text: qsTr("&Image")
            onTriggered: exportImageDialog.open()
        }
        Pentobi.MenuItem {
            text: qsTr("&ASCII Art")
            onTriggered: asciiArtSaveDialog.open()
        }
    }
    MenuSeparator { }
    Pentobi.MenuItem { action: actions.actionQuit }
}
