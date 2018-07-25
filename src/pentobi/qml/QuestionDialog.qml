//-----------------------------------------------------------------------------
/** @file pentobi/qml/QuestionDialog.qml
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

    function openWithCallback(text, acceptedFunc) {
        label.text = text
        _acceptedFunc = acceptedFunc
        open()
    }

    property var _acceptedFunc

    footer: DialogButtonBox {
        Pentobi.ButtonCancel { }
        Pentobi.ButtonOk { }
    }
    onAccepted: _acceptedFunc()

    Item {
        implicitWidth: {
            // Qt 5.11 doesn't correctly handle dialog sizing if dialog (incl.
            // frame) is wider than window and Default style never makes footer
            // wider than content (potentially eliding button texts).
            var w = label.implicitWidth
            // Wrap long text
            w = Math.min(w, font.pixelSize * 25)
            w = Math.max(w, font.pixelSize * 18)
            w = Math.min(w, 0.9 * rootWindow.width)
            return w
        }
        implicitHeight: label.implicitHeight

        Label {
            id: label

            anchors.fill: parent
            wrapMode: Text.Wrap
            topPadding: isDesktop ? 0.3 * font.pixelSize : 0
        }
    }
}
