import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("&Tools")

    MenuItem {
        text: qsTr("&Rating")
        onTriggered: ratingDialog.open()
    }
    MenuItem {
        enabled:  ! isRated
        visible: ! isAndroid || enabled
        text: qsTr("&Analyze Game")
        onTriggered: Logic.analyzeGame()
    }
}
