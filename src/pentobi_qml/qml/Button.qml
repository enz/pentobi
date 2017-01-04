import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.1

/** Button that supports an automatically scaled image.
    The image source should be a SVG file with size 22x22. */
Button {
    id: root

    property string imageSource

    contentItem: Image {
        sourceSize {
            width: Screen.pixelDensity < 5 ? 22 : 44
            height: width
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
