//-----------------------------------------------------------------------------
/** @file pentobi/MessageDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls
import "main.js" as Logic

PentobiDialog {
    property alias text: label.text

    footer: PentobiDialogButtonBox { ButtonOk { } }

    Item {
        implicitWidth:
            Math.max(Math.min(label.implicitWidth,
                              font.pixelSize * 25, maxContentWidth),
                     font.pixelSize * 15, minContentWidth)
        implicitHeight: label.implicitHeight

        Label {
            id: label

            anchors.fill: parent
            wrapMode: Text.Wrap
        }
    }
}
