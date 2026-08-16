//-----------------------------------------------------------------------------
/** @file pentobi/MenuView.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

PentobiMenu {
    title: qsTr("View")

    MenuItem {
        text: qsTr("Appearance")
        onTriggered: appearanceDialog.open()
    }
    MenuItem {
        action: actionComment
    }
    MenuItem {
        action: actionFullscreen
    }
}
