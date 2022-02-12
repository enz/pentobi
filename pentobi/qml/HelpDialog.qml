//-----------------------------------------------------------------------------
/** @file pentobi/qml/HelpDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.15

Dialog {
    width: rootWindow.width; height: rootWindow.height
    modal: true

    HelpViewer {
        anchors.centerIn: parent
        width: rootWindow.width; height: rootWindow.height
    }
}
