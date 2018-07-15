import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("&Tools")

    Pentobi.MenuItem { action: actions.actionRating }
    Pentobi.MenuItem {
        enabled:  ! isRated && ratingModel.numberGames > 0
        text: qsTr("&Clear Rating")
        onTriggered: Logic.clearRating()
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
        text: qsTr("&Analyze Game")
        onTriggered: Logic.analyzeGame()
    }
    Pentobi.MenuItem {
        enabled: analyzeGameModel.elements.length !== 0
        text: qsTr("Clear A&nalysis")
        onTriggered: analyzeGameModel.clear()
    }
}
