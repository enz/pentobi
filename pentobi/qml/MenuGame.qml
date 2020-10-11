//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addShortcut(qsTr("Game"),
                       //: Keyboard shortcut for menu Game. Leave empty for no shortcut.
                       qsTr("G"))

    Pentobi.MenuItem {
        action: actionNew
        text: addShortcut(actionNew.text,
                          //: Keyboard shortcut for menu item New. Leave empty for no shortcut.
                          qsTr("N"))
    }
    Pentobi.MenuItem {
        action: actionNewRated
        text: addShortcut(actionNewRated.text,
                          //: Keyboard shortcut for menu item Rated Game. Leave empty for no shortcut.
                          qsTr("R"))
    }
    MenuSeparator { }
    Action {
        text: addShortcut(qsTr("Game Variant…"),
                          //: Keyboard shortcut for menu item Game Variant. Leave empty for no shortcut.
                          qsTr("V"))
        onTriggered: gameVariantDialog.open()
    }
    Pentobi.MenuItem {
        action: actionGameInfo
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Game Info. Leave empty for no shortcut.
                          qsTr("I"))
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actionUndo
        text: addShortcut(actionUndo.text,
                          //: Keyboard shortcut for menu item Undo. Leave empty for no shortcut.
                          qsTr("U"))
    }
    Pentobi.MenuItem {
        action: actionFindMove
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Find Move. Leave empty for no shortcut.
                          qsTr("F"))
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actionOpen
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Open. Leave empty for no shortcut.
                          qsTr("O"))
    }
    MenuRecentFiles { }
    Action {
        text: addShortcut(qsTr("Open Clipboard"),
                          //: Keyboard shortcut for menu item Open Clipboard. Leave empty for no shortcut.
                          qsTr("C"))
        onTriggered: Logic.openClipboard()
    }
    Pentobi.MenuItem {
        action: actionSave
        enabled: actionSave.enabled && gameModel.file !== ""
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Save. Leave empty for no shortcut.
                          qsTr("S"))
    }
    Pentobi.MenuItem {
        action: actionSaveAs
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Save As. Leave empty for no shortcut.
                          qsTr("A"))
    }
    MenuExport { }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actionQuit
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Quit. Leave empty for no shortcut.
                          qsTr("Q"))
    }
}
