//-----------------------------------------------------------------------------
/** @file pentobi/MenuTools.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls
import "main.js" as Logic

PentobiMenu {
    title: qsTr("Tools")

    PentobiMenuItem {
        text: qsTr("Rating")
        onTriggered: Logic.rating()
    }
    PentobiMenuItem {
        enabled: ! isRated && ratingModel.numberGames > 0
        text: qsTr("Clear Rating")
        onTriggered: Logic.clearRating()
    }
    MenuSeparator { }
    PentobiMenuItem {
        enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
        text: qsTr("Analyze Game…")
        onTriggered: analyzeDialog.open()
    }
    PentobiMenuItem {
        enabled: analyzeGameModel.elements.length !== 0
        text: qsTr("Clear Analysis")
        onTriggered: {
            analyzeGameModel.clear()
            gameView.deleteAnalysis()
        }
    }
}
