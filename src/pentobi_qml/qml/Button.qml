import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.2

/** Button that supports an automatically scaled image.
    The image source should be a SVG file with size 22x22. */
Button {
    id: root

    property string imageSource

    contentItem: Image {
        sourceSize {
            // We currently use 22x22 SVG files, try to use 22x22 or 44x44, unless
            // very high DPI
            width: Screen.pixelDensity < 5 ? 22 : Screen.pixelDensity < 10 ? 44 : 5 * Screen.pixelDensity
            height: Screen.pixelDensity < 5 ? 22 : Screen.pixelDensity < 10 ? 44 : 5 * Screen.pixelDensity
        }
        fillMode: Image.PreserveAspectFit
        source: imageSource
        opacity: root.enabled ? 1 : 0.4
    }
    background: Rectangle {
        anchors.fill: root
        visible: pressed
        color: theme.backgroundButtonPressed
    }
}
