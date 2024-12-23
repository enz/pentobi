//-----------------------------------------------------------------------------
/** @file pentobi/MessageDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

PentobiDialog {
    id: root

    property alias text: label.text

    footer: PentobiDialogButtonBox { ButtonOk { } }

    Item {
        implicitWidth:
            Math.max(Math.min(label.implicitWidth,
                              root.font.pixelSize * 25, root.maxContentWidth),
                     root.font.pixelSize * 15, root.minContentWidth)
        implicitHeight: label.implicitHeight

        Label {
            id: label

            anchors.fill: parent
            wrapMode: Text.Wrap
        }
    }
}
