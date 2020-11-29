//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuGame.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
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
    Action {
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
    Action {
        text: qsTr("Open Clipboard")
        onTriggered: Logic.openClipboard()
    }
    PentobiMenuItem {
        action: actionSave
        enabled: actionSave.enabled && gameModel.file !== ""
    }
    PentobiMenuItem {
        action: actionSaveAs
    }
    MenuExport { relativeWidth: 10 }
    MenuSeparator { }
    PentobiMenuItem {
        action: actionQuit
    }
}
