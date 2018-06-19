import QtQuick.Controls 2.2
import QtQuick.Window 2.0

Menu {
    cascade: false
    // Would like to define width depending on font size, but how?
    width: Math.min(Screen.pixelDensity * 50, rootWindow.width)
}
