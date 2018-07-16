import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Dialog {
    id: root

    property alias text: label.text
    property var acceptedFunc

    function openWithCallback(text, acceptedFunc) {
        label.text = text
        if (root.acceptedFunc)
            accepted.disconnect(root.acceptedFunc)
        accepted.connect(acceptedFunc)
        root.acceptedFunc = acceptedFunc
        timer.restart()
    }

    footer: OkCancelButtons { }

    Column
    {
        width: Math.min(label.font.pixelSize * 30, 0.9 * rootWindow.width)

        Label {
            id: label

            width: 0.9 * parent.width
            wrapMode: Text.Wrap
        }
    }

    // Used to open the dialog in a new event from the Qt event loop to prevent
    // problems if openWithCallback() is called in the current onAccepted
    // callback (e.g. two questions shown in a row) otherwise the dialog
    // won't be visible.
    Timer {
        id: timer

        interval: 0
        onTriggered: open()
    }
}
