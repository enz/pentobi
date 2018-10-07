//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuComputer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Computer"),
                       //: Mnemonic for menu Computer. Leave empty for no mnemonic.
                       qsTr("C"))

    Pentobi.MenuItem {
        action: actionComputerSettings
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Computer Settings. Leave empty for no mnemonic.
                          qsTr("S"))
    }
    Pentobi.MenuItem {
        action: actionPlay
        text: addMnemonic(actionPlay.text,
                          //: Mnemonic for menu item Play. Leave empty for no mnemonic.
                          qsTr("P"))
    }
    Pentobi.MenuItem {
        action: actionPlaySingle
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Play Move. Leave empty for no mnemonic.
                          qsTr("M"))
    }
    Pentobi.MenuItem {
        action: actionStop
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Stop. Leave empty for no mnemonic.
                          qsTr("O"))
    }
}
