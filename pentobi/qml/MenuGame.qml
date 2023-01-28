//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls
import "Main.js" as Logic

PentobiMenu {
    title: qsTr("Game")

    MenuItem {
        action: actionNew
    }
    MenuItem {
        action: actionNewRated
    }
    MenuSeparator { }
    Action {
        text: qsTr("Game Variant…")
        onTriggered: gameVariantDialog.open()
    }
    MenuItem {
        action: actionGameInfo
    }
    MenuSeparator { }
    MenuItem {
        action: actionUndo
    }
    MenuItem {
        action: actionFindMove
    }
    MenuSeparator { }
    MenuItem {
        action: actionOpen
    }
    MenuRecentFiles { }
    Action {
        text: qsTr("Open Clipboard")
        onTriggered: Logic.openClipboard()
    }
    MenuItem {
        action: actionSave
        enabled: actionSave.enabled && gameModel.file !== ""
    }
    MenuItem {
        action: actionSaveAs
    }
    MenuExport { relativeWidth: 10 }
    MenuSeparator { }
    MenuItem {
        action: actionQuit
    }
}
