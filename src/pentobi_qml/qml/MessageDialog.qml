import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0

// Used instead of MessageDialog from QtQuick.Dialogs because those have
// a different font and buttons on Android than other dialogs from
// QtQuick.Dialogs, and we want a coherent dialog style.
Dialog {
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

    onVisibleChanged: if (! visible) gameDisplay.forceActiveFocus() // QTBUG-48456

    // Outer column is a workaround for a bug in Qt on Android (last tested
    // with Qt 5.8beta), which causes the label to be too wide.
    Column
    {
        width: Math.min(Screen.pixelDensity * 80, 0.95 * Screen.width)

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
