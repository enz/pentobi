import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0

// Used instead of MessageDialog from QtQuick.Dialogs because those have
// a different font and buttons on Android than other dialogs from
// QtQuick.Dialogs, and we want a coherent dialog style.
Dialog {
    property alias text: label.text

    Column
    {
        width: Math.min(Screen.pixelDensity * 120, 0.95 * Screen.width)
        spacing: Screen.pixelDensity * 2

        Label {
            id: label

            width: 0.9 * parent.width
            wrapMode: Text.Wrap
        }
    }
}
