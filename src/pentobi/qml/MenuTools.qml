//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuTools.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Tools"),
                       //: Mnemonic for menu Tools. Leave empty for no mnemonic.
                       qsTr("T"))

    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Rating"),
                          //: Mnemonic for menu item Rating. Leave empty for no mnemonic.
                          qsTr("R"))
        onTriggered: Logic.rating()
    }
    Action {
        enabled: ! isRated && ratingModel.numberGames > 0
        text: addMnemonic(qsTr("Clear Rating"),
                          //: Mnemonic for menu item Clear Rating. Leave empty for no mnemonic.
                          qsTr("C"))
        onTriggered: Logic.clearRating()
    }
    Pentobi.MenuSeparator { }
    Action {
        enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
        // Text needs to end with ellipsis on desktop because it opens a
        // dialog asking for analysis speed, but not on Android
        text: addMnemonic(isAndroid ? qsTr("Analyze Game") : qsTr("Analyze Game…"),
                          //: Mnemonic for menu item Analyze Game. Leave empty for no mnemonic.
                          qsTr("A"))
        onTriggered: {
            if (isAndroid)
                Logic.analyzeGame(3000)
            else
                analyzeDialog.open()
        }
    }
    Action {
        enabled: analyzeGameModel.isRunning
        text: addMnemonic(qsTr("Abort Analysis"),
                          //: Mnemonic for menu item Abort Analysis. Leave empty for no mnemonic.
                          qsTr("O"))
        onTriggered: Logic.cancelRunning(true)
    }
    Action {
        enabled: analyzeGameModel.elements.length !== 0
        text: addMnemonic(qsTr("Clear Analysis"),
                          //: Mnemonic for menu item Clear Analysis. Leave empty for no mnemonic.
                          qsTr("E"))
        onTriggered:
            Qt.callLater(function() { analyzeGameModel.clear() }) // QTBUG-69682
    }
}
