//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuComputer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import "." as Pentobi

Pentobi.Menu {
    title: qsTr("Computer")

    Pentobi.MenuItem {
        action: actionComputerSettings
    }
    Pentobi.MenuItem {
        action: actionPlay
    }
    Pentobi.MenuItem {
        action: actionPlaySingle
    }
    Pentobi.MenuItem {
        action: actionStop
    }
}
