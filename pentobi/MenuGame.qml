//-----------------------------------------------------------------------------
/** @file pentobi/MenuGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls
import "Main.js" as Logic

PentobiMenu {
    title: qsTr("Game")

    PentobiMenuItem {
        action: actionNew
    }
    PentobiMenuItem {
        action: actionNewRated
    }
    MenuSeparator { }
    PentobiMenuItem {
        text: qsTr("Game Variant…")
        onTriggered: gameVariantDialog.open()
    }
    PentobiMenuItem {
        action: actionGameInfo
    }
    MenuSeparator { }
    PentobiMenuItem {
        action: actionUndo
    }
    PentobiMenuItem {
        action: actionFindMove
    }
    MenuSeparator { }
    PentobiMenuItem {
        action: actionOpen
    }
    MenuRecentFiles { }
    PentobiMenuItem {
        action: actionSave
        enabled: actionSave.enabled && gameModel.file !== ""
    }
    PentobiMenuItem {
        action: actionSaveAs
    }
    PentobiMenuItem {
        text: qsTr("Export Image…")
        onTriggered: exportImageDialog.open()
    }
    MenuSeparator { }
    PentobiMenuItem {
        action: actionQuit
    }
}
