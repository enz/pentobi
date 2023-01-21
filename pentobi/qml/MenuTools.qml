//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuTools.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls
import "Main.js" as Logic

PentobiMenu {
    title: qsTr("Tools")

    PentobiMenuItem {
        text: qsTr("Rating")
        onTriggered: Logic.rating()
    }
    Action {
        enabled: ! isRated && ratingModel.numberGames > 0
        text: qsTr("Clear Rating")
        onTriggered: Logic.clearRating()
    }
    PentobiMenuSeparator { }
    Action {
        enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
        text: qsTr("Analyze Game…")
        onTriggered: analyzeDialog.open()
    }
    Action {
        enabled: analyzeGameModel.elements.length !== 0
        text: qsTr("Clear Analysis")
        onTriggered: {
            analyzeGameModel.clear()
            gameView.deleteAnalysis()
        }
    }
}
