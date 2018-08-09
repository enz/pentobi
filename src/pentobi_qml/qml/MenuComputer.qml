import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: Logic.removeShortcut(qsTr("&Computer"))

    MenuItem {
        text: Logic.removeShortcut(qsTr("Computer &Colors"))
        onTriggered: Logic.showComputerColorDialog()
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&Play"))
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: Logic.computerPlay()
    }
    MenuSeparator { }
    MenuItem {
        text: Logic.removeShortcut(qsTr("Play &Single Move"))
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: { isPlaySingleMoveRunning = true; Logic.genMove() }
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("St&op"))
        enabled: playerModel.isGenMoveRunning && ! isRated
        onTriggered: Logic.cancelRunning()
    }
    MenuSeparator { }
    Pentobi.Menu {
        title:
            switch (gameModel.gameVariant)
            {
            case "classic": return Logic.removeShortcut(qsTr("&Level (Classic, 4 Players)"))
            case "classic_2": return Logic.removeShortcut(qsTr("&Level (Classic, 2 Players)"))
            case "classic_3": return Logic.removeShortcut(qsTr("&Level (Classic, 3 Players)"))
            case "duo": return Logic.removeShortcut(qsTr("&Level (Duo)"))
            case "junior": return Logic.removeShortcut(qsTr("&Level (Junior)"))
            case "trigon": return Logic.removeShortcut(qsTr("&Level (Trigon, 4 Players)"))
            case "trigon_2": return Logic.removeShortcut(qsTr("&Level (Trigon, 2 Players)"))
            case "trigon_3": return Logic.removeShortcut(qsTr("&Level (Trigon, 3 Players)"))
            case "nexos": return Logic.removeShortcut(qsTr("&Level (Nexos, 4 Players)"))
            case "nexos_2": return Logic.removeShortcut(qsTr("&Level (Nexos, 2 Players)"))
            case "callisto": return Logic.removeShortcut(qsTr("&Level (Callisto, 4 Players)"))
            case "callisto_2": return Logic.removeShortcut(qsTr("&Level (Callisto, 2 Players, 2 Colors)"))
            case "callisto_2_4": return Logic.removeShortcut(qsTr("&Level (Callisto, 2 Players, 4 Colors)"))
            case "callisto_3": return Logic.removeShortcut(qsTr("&Level (Callisto, 3 Players)"))
            case "gembloq": return Logic.removeShortcut(qsTr("&Level (GembloQ, 4 Players)"))
            case "gembloq_2": return Logic.removeShortcut(qsTr("&Level (GembloQ, 2 Players, 2 Colors)"))
            case "gembloq_2_4": return Logic.removeShortcut(qsTr("&Level (GembloQ, 2 Players, 4 Colors)"))
            case "gembloq_3": return Logic.removeShortcut(qsTr("&Level (GembloQ, 3 Players)"))
            }

        MenuItemLevel { level: 1 }
        MenuItemLevel { level: 2 }
        MenuItemLevel { level: 3 }
        MenuItemLevel { level: 4 }
        MenuItemLevel { level: 5 }
        MenuItemLevel { level: 6 }
        MenuItemLevel { level: 7 }
    }
}