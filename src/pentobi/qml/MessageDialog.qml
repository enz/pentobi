//-----------------------------------------------------------------------------
/** @file pentobi/qml/MessageDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Dialog {
    id: root

    property alias text: label.text

    footer: Pentobi.DialogButtonBox { ButtonOk { } }

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
