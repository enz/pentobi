//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuTools.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addShortcut(qsTr("Tools"),
                       //: Keyboard shortcut for menu Tools. Leave empty for no shortcut.
                       qsTr("T"))

    Pentobi.MenuItem {
        text: addShortcut(qsTr("Rating"),
                          //: Keyboard shortcut for menu item Rating. Leave empty for no shortcut.
                          qsTr("R"))
        onTriggered: Logic.rating()
    }
    Action {
        enabled: ! isRated && ratingModel.numberGames > 0
        text: addShortcut(qsTr("Clear Rating"),
                          //: Keyboard shortcut for menu item Clear Rating. Leave empty for no shortcut.
                          qsTr("C"))
        onTriggered: Logic.clearRating()
    }
    Pentobi.MenuSeparator { }
    Action {
        enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
        // Text needs to end with ellipsis on desktop because it opens a
        // dialog asking for analysis speed, but not on Android
        text: addShortcut(isAndroid ? qsTr("Analyze Game") : qsTr("Analyze Game…"),
                          //: Keyboard shortcut for menu item Analyze Game. Leave empty for no shortcut.
                          qsTr("A"))
        onTriggered: {
            if (isAndroid)
                Logic.analyzeGame(3000)
            else
                analyzeDialog.open()
        }
    }
    Action {
        enabled: analyzeGameModel.elements.length !== 0
        text: addShortcut(qsTr("Clear Analysis"),
                          //: Keyboard shortcut for menu item Clear Analysis. Leave empty for no shortcut.
                          qsTr("E"))
        onTriggered:
            Qt.callLater(function() { // QTBUG-69682
                analyzeGameModel.clear()
                gameDisplay.deleteAnalysis()
            })
    }
}
