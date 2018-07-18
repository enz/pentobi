import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Computer"),
                       //: Mnemonic for menu Computer. Leave empty for no mnemonic.
                       qsTr("C"))

    Pentobi.MenuItem {
        action: actions.actionComputerSettings
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Computer Settings. Leave empty for no mnemonic.
                          qsTr("S"))
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
                          qsTr("O"))
        enabled: (playerModel.isGenMoveRunning
                  || delayedCheckComputerMove.running
                  || analyzeGameModel.isRunning)
                 && ! isRated
        onTriggered: Logic.cancelRunning()
    }
}
