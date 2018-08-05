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
        action: actions.newGame
        text: addMnemonic(actions.newGame.text,
                          //: Mnemonic for menu item New. Leave empty for no mnemonic.
                          qsTr("N"))
    }
    Pentobi.MenuItem {
        action: actions.newGameRated
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Rated Game. Leave empty for no mnemonic.
                          qsTr("R"))
    }
    Pentobi.MenuSeparator { }
    Action {
        text: addMnemonic(qsTr("Game Variant…"),
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
        action: actions.undo
        text: addMnemonic(actions.undo.text,
                          //: Mnemonic for menu item Undo. Leave empty for no mnemonic.
                          qsTr("U"))
    }
    Pentobi.MenuItem {
        action: actions.findMove
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Find Move. Leave empty for no mnemonic.
                          qsTr("F"))
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.open
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Open. Leave empty for no mnemonic.
                          qsTr("O"))
    }
    MenuRecentFiles { }
    Action {
        text: addMnemonic(qsTr("Open Clipboard"),
                          //: Mnemonic for menu item Open Clipboard. Leave empty for no mnemonic.
                          qsTr("C"))
        onTriggered: Logic.openClipboard()
    }
    Pentobi.MenuItem {
        action: actions.save
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Save. Leave empty for no mnemonic.
                          qsTr("S"))
    }
    Pentobi.MenuItem {
        action: actions.saveAs
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Save As. Leave empty for no mnemonic.
                          qsTr("A"))
    }
    MenuExport { }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.quit
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Quit. Leave empty for no mnemonic.
                          qsTr("Q"))
    }
}
