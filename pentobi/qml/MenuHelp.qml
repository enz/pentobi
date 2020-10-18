//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuHelp.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("Help")

    Pentobi.MenuItem {
        action: actionHelp
    }
    Action {
        text: qsTr("Report Bug")
        onTriggered: Qt.openUrlExternally("https://github.com/enz/pentobi/issues")
    }
    Action {
        text: qsTr("About Pentobi")
        onTriggered: aboutDialog.open()
    }
}
