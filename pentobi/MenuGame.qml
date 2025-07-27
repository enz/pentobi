//-----------------------------------------------------------------------------
/** @file pentobi/MenuGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    title: qsTr("Game")

    MenuItem {
        action: actionNew
    }
    MenuItem {
        action: actionNewRated
    }
    MenuSeparator { }
    MenuItem {
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
    MenuItem {
        action: actionSave
        enabled: actionSave.enabled && gameModel.file !== ""
    }
    MenuItem {
        action: actionSaveAs
    }
    MenuItem {
        text: qsTr("Export Image…")
        onTriggered: exportImageDialog.open()
    }
    MenuSeparator { }
    MenuItem {
        action: actionQuit
    }
}
