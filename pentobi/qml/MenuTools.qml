//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuTools.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("Tools")

    Pentobi.MenuItem {
        text: qsTr("Rating")
        onTriggered: Logic.rating()
    }
    Action {
        enabled: ! isRated && ratingModel.numberGames > 0
        text: qsTr("Clear Rating")
        onTriggered: Logic.clearRating()
    }
    Pentobi.MenuSeparator { }
    Action {
        enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
        text: qsTr("Analyze Game…")
        onTriggered: analyzeDialog.open()
    }
    Action {
        enabled: analyzeGameModel.elements.length !== 0
        text: qsTr("Clear Analysis")
        onTriggered:
            Qt.callLater(function() { // QTBUG-69682
                analyzeGameModel.clear()
                gameView.deleteAnalysis()
            })
    }
}
