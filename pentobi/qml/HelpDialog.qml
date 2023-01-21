//-----------------------------------------------------------------------------
/** @file pentobi/qml/HelpDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

Dialog {
    width: rootWindow.contentItem.width
    height: rootWindow.contentItem.height
    padding: 0
    modal: true

    HelpViewer {
        anchors.fill: parent
    }
}
