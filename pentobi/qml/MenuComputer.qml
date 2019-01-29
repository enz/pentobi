//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuComputer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import "." as Pentobi

Pentobi.Menu {
    title: addShortcut(qsTr("Computer"),
                       //: Keyboard shortcut for menu Computer. Leave empty for no shortcut.
                       qsTr("C"))

    Pentobi.MenuItem {
        action: actionComputerSettings
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Computer Settings. Leave empty for no shortcut.
                          qsTr("S"))
    }
    Pentobi.MenuItem {
        action: actionPlay
        text: addShortcut(actionPlay.text,
                          //: Keyboard shortcut for menu item Play. Leave empty for no shortcut.
                          qsTr("P"))
    }
    Pentobi.MenuItem {
        action: actionPlaySingle
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Play Move. Leave empty for no shortcut.
                          qsTr("M"))
    }
    Pentobi.MenuItem {
        action: actionStop
        text: addShortcut(action.text,
                          //: Keyboard shortcut for menu item Stop. Leave empty for no shortcut.
                          qsTr("O"))
    }
}
