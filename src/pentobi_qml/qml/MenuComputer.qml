import QtQml 2.2
import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("&Computer")

    Pentobi.MenuItem { action: actions.actionComputerColors }
    Pentobi.MenuItem { action: actions.actionPlay }
    MenuSeparator { }
    Pentobi.MenuItem { action: actions.actionPlaySingle }
    Pentobi.MenuItem {
        text: qsTr("St&op")
        enabled: (playerModel.isGenMoveRunning
                  || delayedCheckComputerMove.running
                  || analyzeGameModel.isRunning)
                 && ! isRated
        onTriggered: Logic.cancelRunning()
    }
    MenuSeparator { }
    Pentobi.Menu {
        id: menuLevel

        title:
            switch (gameModel.gameVariant)
            {
            case "classic": return qsTr("&Level (Classic, 4 Players)")
            case "classic_2": return qsTr("&Level (Classic, 2 Players)")
            case "classic_3": return qsTr("&Level (Classic, 3 Players)")
            case "duo": return qsTr("&Level (Duo)")
            case "junior": return qsTr("&Level (Junior)")
            case "trigon": return qsTr("&Level (Trigon, 4 Players)")
            case "trigon_2": return qsTr("&Level (Trigon, 2 Players)")
            case "trigon_3": return qsTr("&Level (Trigon, 3 Players)")
            case "nexos": return qsTr("&Level (Nexos, 4 Players)")
            case "nexos_2": return qsTr("&Level (Nexos, 2 Players)")
            case "callisto": return qsTr("&Level (Callisto, 4 Players)")
            case "callisto_2": return qsTr("&Level (Callisto, 2 Players, 2 Colors)")
            case "callisto_2_4": return qsTr("&Level (Callisto, 2 Players, 4 Colors)")
            case "callisto_3": return qsTr("&Level (Callisto, 3 Players)")
            case "gembloq": return qsTr("&Level (GembloQ, 4 Players)")
            case "gembloq_2": return qsTr("&Level (GembloQ, 2 Players, 2 Colors)")
            case "gembloq_2_4": return qsTr("&Level (GembloQ, 2 Players, 4 Colors)")
            case "gembloq_3": return qsTr("&Level (GembloQ, 3 Players)")
            }

        Instantiator {
            model: playerModel.maxLevel
            onObjectAdded: menuLevel.insertItem(index, object)
            onObjectRemoved: menuLevel.removeItem(object)

            MenuItemLevel { level: index + 1 }
        }
    }
}
