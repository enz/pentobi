import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0

// Used instead of MessageDialog from QtQuick.Dialogs because those have
// a different font and buttons on Android than other dialogs from
// QtQuick.Dialogs, and we want a coherent dialog style.
Dialog {
    property alias text: label.text

    // Outer column is a workaround for a bug in Qt on Android (last tested
    // with Qt 5.8beta), which causes the label to be too wide.
    Column
    {
        width: Math.min(Screen.pixelDensity * 120, 0.95 * Screen.width)

        Label {
            id: label

            width: 0.9 * parent.width
            wrapMode: Text.Wrap
        }
    }
}
