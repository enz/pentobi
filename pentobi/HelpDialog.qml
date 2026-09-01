//-----------------------------------------------------------------------------
/** @file pentobi/HelpDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
// Enforce Basic style to avoid rounded dialog corners in Material style
import QtQuick.Controls.Basic

// Dialog used instead of a window for showing help in mobile view
Dialog {
    id: root

    property bool isDark

    width: rootWindow.contentItem.width
    height: rootWindow.contentItem.height
    padding: 0
    modal: true
    focus: ! workaroundOskBug

    Rectangle {
        anchors.fill: parent
        color: theme.colorBackground

        HelpViewer {
            id: helpViewer

            isDark: root.isDark
            anchors.fill: parent
            onCloseClicked: close()
        }
    }
}