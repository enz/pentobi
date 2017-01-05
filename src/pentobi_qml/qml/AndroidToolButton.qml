import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.0

ToolButton {
    property string imageSource

    Image {
        // We currently use 22x22 SVG files, try to use 22x22 or 44x44, unless
        // very high DPI
        width: Screen.pixelDensity < 5 ? 22 : Screen.pixelDensity < 10 ? 44 : 5 * Screen.pixelDensity
        height: width
        sourceSize { width: width; height: height }
        anchors.centerIn: parent
        source: imageSource
        cache: false
    }
}
