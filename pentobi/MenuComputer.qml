//-----------------------------------------------------------------------------
/** @file pentobi/MenuComputer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    title: qsTr("Computer")

    MenuItem {
        action: actionComputerSettings
    }
    MenuItem {
        action: actionPlay
    }
    MenuItem {
        action: actionPlaySingle
    }
    MenuItem {
        action: actionStop
    }
}
