//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuHelp.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    title: qsTr("Help")

    PentobiMenuItem {
        action: actionHelp
    }
    Action {
        text: qsTr("About Pentobi")
        onTriggered: aboutDialog.open()
    }
}
