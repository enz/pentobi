import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: Logic.removeShortcut(qsTr("&Tools"))

    Pentobi.MenuItem { action: actions.actionRating }
    Pentobi.MenuItem {
        enabled:  ! isRated && ratingModel.numberGames > 0
        text: Logic.removeShortcut(qsTr("&Clear Rating"))
        onTriggered: Logic.clearRating()
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        enabled: ! isRated && (gameModel.canGoBackward || gameModel.canGoForward)
        text: Logic.removeShortcut(qsTr("&Analyze Game"))
        onTriggered: Logic.analyzeGame()
    }
    Pentobi.MenuItem {
        enabled: analyzeGameModel.elements.length !== 0
        text: Logic.removeShortcut(qsTr("Clear A&nalysis"))
        onTriggered: analyzeGameModel.clear()
    }
}
