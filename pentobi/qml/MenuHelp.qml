//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuHelp.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3
import "." as Pentobi

Pentobi.Menu {
    title: addShortcut(qsTr("Help"),
                       //: Keyboard shortcut for menu Help. Leave empty for no shortcut.
                       qsTr("H"))

    Pentobi.MenuItem {
        action: actionHelp
        text: addShortcut(actionHelp.text,
                          //: Keyboard shortcut for menu item Pentobi Help. Leave empty for no shortcut.
                          qsTr("P"))
    }
    Action {
        text: addShortcut(qsTr("Report Bug"),
                          //: Keyboard shortcut for menu item Report Bug. Leave empty for no shortcut.
                          qsTr("B"))
        onTriggered: Qt.openUrlExternally("https://github.com/enz/pentobi/issues")
    }
    Action {
        text: addShortcut(qsTr("About Pentobi"),
                          //: Keyboard shortcut for menu item About Pentobi. Leave empty for no shortcut.
                          qsTr("A"))
        onTriggered: aboutDialog.open()
    }
}
