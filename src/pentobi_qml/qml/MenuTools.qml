import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic

Menu {
    title: qsTr("&Tools")

    MenuItem {
        text: qsTr("&Rating")
        onTriggered: {
            // Never reuse RatingDialog
            // See comment in Main.qml at ratingModel.onHistoryChanged
            ratingDialog.source = ""
            ratingDialog.open()
        }
    }
    MenuItem {
        enabled:  ! isRated && ratingModel.numberGames > 0
        text: qsTr("&Clear Rating")
        onTriggered: Logic.clearRating()
    }
    MenuSeparator { }
    MenuItem {
        enabled:  ! isRated
        text: qsTr("&Analyze Game")
        onTriggered: Logic.analyzeGame()
    }
}
