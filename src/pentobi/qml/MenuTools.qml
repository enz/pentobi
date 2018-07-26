//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuTools.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Tools"),
                       //: Mnemonic for menu Tools. Leave empty for no mnemonic.
                       qsTr("T"))

    Pentobi.MenuItem {
        action: actions.actionRating
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Rating. Leave empty for no mnemonic.
                          qsTr("R"))
    }
    Pentobi.MenuItem {
        enabled:  ! isRated && ratingModel.numberGames > 0
        text: addMnemonic(qsTr("Clear Rating"),
                          //: Mnemonic for menu item Clear Rating. Leave empty for no mnemonic.
                          qsTr("C"))
        onTriggered: Logic.clearRating()
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
        text: addMnemonic(qsTr("Analyze Game"),
                          //: Mnemonic for menu item Analyze Game. Leave empty for no mnemonic.
                          qsTr("A"))
        onTriggered: {
            if (isAndroid)
                Logic.analyzeGame(3000)
            else
                analyzeDialog.open()
        }
    }
    Pentobi.MenuItem {
        enabled: analyzeGameModel.isRunning
        text: addMnemonic(qsTr("Abort Analysis"),
                          //: Mnemonic for menu item Abort Analysis. Leave empty for no mnemonic.
                          qsTr("O"))
        onTriggered: Logic.cancelRunning(true)
    }
    Pentobi.MenuItem {
        enabled: analyzeGameModel.elements.length !== 0
        text: addMnemonic(qsTr("Clear Analysis"),
                          //: Mnemonic for menu item Clear Analysis. Leave empty for no mnemonic.
                          qsTr("E"))
        onTriggered: analyzeGameModel.clear()
    }
}
