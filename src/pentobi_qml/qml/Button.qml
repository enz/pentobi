import QtQuick 2.0
import QtQuick.Window 2.0
import Qt.labs.controls 1.0 as Controls2

/** Button that supports an automatically scaled image.
    The image source should be a SVG file with size 22x22. */
Controls2.Button {
    id: root

    property string imageSource

    label: Image {
        sourceSize {
            width: Screen.pixelDensity < 5 ? 22 : 44
            height: width
        }
        anchors.centerIn: parent
        source: imageSource
        opacity: root.enabled ? 1 : 0.4
        cache: false
    }
    background: Rectangle {
        anchors.fill: root
        visible: pressed
        color: theme.backgroundButtonPressed
    }
}
