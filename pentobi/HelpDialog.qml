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
    width: rootWindow.contentItem.width
    height: rootWindow.contentItem.height
    padding: 0
    modal: true

    // Put help viewer in Rectangle to enforce white background, otherwise
    // space below help page is gray (Material style, Qt 6.9)
    Rectangle {
        anchors.fill: parent

        HelpViewer {
            id: helpViewer

            anchors.fill: parent
            onCloseClicked: close()
        }
    }
}