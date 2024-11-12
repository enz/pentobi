//-----------------------------------------------------------------------------
/** @file pentobi/MenuComputer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

PentobiMenu {
    title: qsTr("Computer")

    PentobiMenuItem {
        action: actionComputerSettings
    }
    PentobiMenuItem {
        action: actionPlay
    }
    PentobiMenuItem {
        action: actionPlaySingle
    }
    PentobiMenuItem {
        action: actionStop
    }
}
