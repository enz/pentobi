//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Game"),
                       //: Mnemonic for menu Game. Leave empty for no mnemonic.
                       qsTr("G"))

    Pentobi.MenuItem {
        action: actionNew
        text: addMnemonic(actionNew.text,
                          //: Mnemonic for menu item New. Leave empty for no mnemonic.
                          qsTr("N"))
    }
    Pentobi.MenuItem {
        action: actionNewRated
        text: addMnemonic(actionNewRated.text,
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
        action: actionGameInfo
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Game Info. Leave empty for no mnemonic.
                          qsTr("I"))
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actionUndo
        text: addMnemonic(actionUndo.text,
                          //: Mnemonic for menu item Undo. Leave empty for no mnemonic.
                          qsTr("U"))
    }
    Pentobi.MenuItem {
        action: actionFindMove
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Find Move. Leave empty for no mnemonic.
                          qsTr("F"))
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actionOpen
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
        action: actionSave
        enabled: actionSave.enabled && gameModel.file !== ""
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Save. Leave empty for no mnemonic.
                          qsTr("S"))
    }
    Pentobi.MenuItem {
        action: actionSaveAs
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Save As. Leave empty for no mnemonic.
                          qsTr("A"))
    }
    MenuExport { }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actionQuit
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Quit. Leave empty for no mnemonic.
                          qsTr("Q"))
    }
}
