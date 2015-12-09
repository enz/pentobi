import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("&Computer")

    MenuItem {
        text: qsTr("Computer &Colors")
        onTriggered: Logic.showComputerColorDialog()
    }
    MenuItem {
        text: qsTr("&Play")
        enabled: ! gameModel.isGameOver
        visible: ! isAndroid || enabled
        onTriggered: Logic.computerPlay()
    }
    Menu {
        title: qsTr("&Level")

        ExclusiveGroup { id: levelGroup }
        MenuItemLevel { level: 1 }
        MenuItemLevel { level: 2 }
        MenuItemLevel { level: 3 }
        MenuItemLevel { level: 4 }
        MenuItemLevel { level: 5 }
        MenuItemLevel { level: 6 }
        MenuItemLevel { level: 7 }
    }
}
