import QtQml 2.2
import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Computer"),
                       //: Mnemonic for menu Computer. Leave empty for no mnemonic.
                       qsTr("C"))

    Pentobi.MenuItem {
        action: actions.actionComputerColors
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Computer Colors. Leave empty for no mnemonic.
                          qsTr("O"))
    }
    Pentobi.MenuItem {
        action: actions.actionPlay
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Play. Leave empty for no mnemonic.
                          qsTr("P"))
    }
    Pentobi.MenuItem {
        action: actions.actionPlaySingle
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Play Single Move. Leave empty for no mnemonic.
                          qsTr("M"))
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Stop"),
                          //: Mnemonic for menu item Stop. Leave empty for no mnemonic.
                          qsTr("S"))
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
            case "classic":
                return addMnemonic(qsTr("Level (Classic, 4 Players)"),
                                   //: Mnemonic for menu item Level. Leave empty for no mnemonic.
                                   qsTr("L"))
            case "classic_2": return addMnemonic(qsTr("Level (Classic, 2 Players)"), qsTr("L"))
            case "classic_3": return addMnemonic(qsTr("Level (Classic, 3 Players)"), qsTr("L"))
            case "duo": return addMnemonic(qsTr("Level (Duo)"), qsTr("L"))
            case "junior": return addMnemonic(qsTr("Level (Junior)"), qsTr("L"))
            case "trigon": return addMnemonic(qsTr("Level (Trigon, 4 Players)"), qsTr("L"))
            case "trigon_2": return addMnemonic(qsTr("Level (Trigon, 2 Players)"), qsTr("L"))
            case "trigon_3": return addMnemonic(qsTr("Level (Trigon, 3 Players)"), qsTr("L"))
            case "nexos": return addMnemonic(qsTr("Level (Nexos, 4 Players)"), qsTr("L"))
            case "nexos_2": return addMnemonic(qsTr("Level (Nexos, 2 Players)"), qsTr("L"))
            case "callisto": return addMnemonic(qsTr("Level (Callisto, 4 Players)"), qsTr("L"))
            case "callisto_2": return addMnemonic(qsTr("Level (Callisto, 2 Players, 2 Colors)"), qsTr("L"))
            case "callisto_2_4": return addMnemonic(qsTr("Level (Callisto, 2 Players, 4 Colors)"), qsTr("L"))
            case "callisto_3": return addMnemonic(qsTr("Level (Callisto, 3 Players)"), qsTr("L"))
            case "gembloq": return addMnemonic(qsTr("Level (GembloQ, 4 Players)"), qsTr("L"))
            case "gembloq_2": return addMnemonic(qsTr("Level (GembloQ, 2 Players, 2 Colors)"), qsTr("L"))
            case "gembloq_2_4": return addMnemonic(qsTr("Level (GembloQ, 2 Players, 4 Colors)"), qsTr("L"))
            case "gembloq_3": return addMnemonic(qsTr("Level (GembloQ, 3 Players)"), qsTr("L"))
            }

        Instantiator {
            model: playerModel.maxLevel
            onObjectAdded: menuLevel.insertItem(index, object)
            onObjectRemoved: menuLevel.removeItem(object)

            MenuItemLevel { level: index + 1 }
        }
    }
}
