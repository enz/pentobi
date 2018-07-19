import QtQml 2.2
import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Game"),
                       //: Mnemonic for menu Game. Leave empty for no mnemonic.
                       qsTr("G"))

    Pentobi.MenuItem {
        action: actions.actionNew
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item New. Leave empty for no mnemonic.
                          qsTr("N"))
    }
    Pentobi.MenuItem {
        action: actions.actionNewRated
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Rated Game. Leave empty for no mnemonic.
                          qsTr("R"))
    }
    MenuSeparator { }
    MenuItem {
        text: addMnemonic(qsTr("Game Variant..."),
                          //: Mnemonic for menu item Game Variant. Leave empty for no mnemonic.
                          qsTr("V"))
        onTriggered: gameVariantDialog.open()
    }
    Pentobi.MenuItem {
        action: actions.actionGameInfo
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Game Info. Leave empty for no mnemonic.
                          qsTr("I"))
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.actionUndo
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Undo. Leave empty for no mnemonic.
                          qsTr("U"))
    }
    Pentobi.MenuItem {
        action: actions.actionFindMove
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Find Move. Leave empty for no mnemonic.
                          qsTr("F"))
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.actionOpen
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Open. Leave empty for no mnemonic.
                          qsTr("O"))
    }
    Pentobi.Menu {
        id: recentFiles

        title: addMnemonic(qsTr("Open Recent"),
                           //: Mnemonic for menu Open Recent. Leave empty for no mnemonic.
                           qsTr("P"))
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
        text: addMnemonic(qsTr("Open from Clipboard"),
                          //: Mnemonic for menu item Open from Clipboard. Leave empty for no mnemonic.
                          qsTr("C"))
        onTriggered: Logic.openClipboard()
    }
    Pentobi.MenuItem {
        action: actions.actionSave
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Save. Leave empty for no mnemonic.
                          qsTr("S"))
    }
    Pentobi.MenuItem {
        action: actions.actionSaveAs
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Save As. Leave empty for no mnemonic.
                          qsTr("A"))
    }
    MenuExport { }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.actionQuit
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Quit. Leave empty for no mnemonic.
                          qsTr("Q"))
    }
}
