//-----------------------------------------------------------------------------
/** @file pentobi/MenuHelp.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    title: qsTr("Help")

    MenuItem {
        action: actionHelp
    }
    MenuItem {
        text: qsTr("About Pentobi")
        onTriggered: aboutDialog.open()
    }
}
