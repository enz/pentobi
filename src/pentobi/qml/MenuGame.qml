//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Game"),
                       //: Mnemonic for menu Game. Leave empty for no mnemonic.
                       qsTr("G"))

    Pentobi.MenuItem {
        enabled: actions.actionNew.enabled
        text: addMnemonic(actions.actionNew.text,
                          //: Mnemonic for menu item New. Leave empty for no mnemonic.
                          qsTr("N"))
        shortcut: actions.actionNew.shortcut
        onTriggered: {
            actions.actionNew.onTriggered()
            dismiss() // QTBUG-69682
        }
    }
    Pentobi.MenuItem {
        action: actions.actionNewRated
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Rated Game. Leave empty for no mnemonic.
                          qsTr("R"))
    }
    Pentobi.MenuSeparator { }
    Action {
        text: addMnemonic(qsTr("Game Variant..."),
                          //: Mnemonic for menu item Game Variant. Leave empty for no mnemonic.
                          qsTr("V"))
        onTriggered: gameVariantDialog.open()
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Game Info"),
                          //: Mnemonic for menu item Game Info. Leave empty for no mnemonic.
                          qsTr("I"))
        onTriggered: gameInfoDialog.open()
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        enabled: actions.actionUndo.enabled
        text: addMnemonic(actions.actionUndo.text,
                          //: Mnemonic for menu item Undo. Leave empty for no mnemonic.
                          qsTr("U"))
        onTriggered: {
            actions.actionUndo.onTriggered()
            dismiss() // QTBUG-69682
        }
    }
    Pentobi.MenuItem {
        action: actions.actionFindMove
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Find Move. Leave empty for no mnemonic.
                          qsTr("F"))
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.actionOpen
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Open. Leave empty for no mnemonic.
                          qsTr("O"))
    }
    MenuRecentFiles { }
    Action {
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
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.actionQuit
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Quit. Leave empty for no mnemonic.
                          qsTr("Q"))
    }
}
