//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("Game")

    Pentobi.MenuItem {
        action: actionNew
    }
    Pentobi.MenuItem {
        action: actionNewRated
    }
    Pentobi.MenuSeparator { }
    Action {
        text: qsTr("Game Variant…")
        onTriggered: gameVariantDialog.open()
    }
    Pentobi.MenuItem {
        action: actionGameInfo
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actionUndo
    }
    Pentobi.MenuItem {
        action: actionFindMove
    }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actionOpen
    }
    MenuRecentFiles { }
    Action {
        text: qsTr("Open Clipboard")
        onTriggered: Logic.openClipboard()
    }
    Pentobi.MenuItem {
        action: actionSave
        enabled: actionSave.enabled && gameModel.file !== ""
    }
    Pentobi.MenuItem {
        action: actionSaveAs
    }
    MenuExport { relativeWidth: 10 }
    Pentobi.MenuSeparator { }
    Pentobi.MenuItem {
        action: actionQuit
    }
}
