import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.0

/** ToolButton with a dynamically sized image.
    Works around missing support for multi-size icons in QtQuick.Controls 1.1.
    The image source should be a SVG file with size 22x22. */
ToolButton {
    property string imageSource

    Image {
        width: Screen.pixelDensity < 5 ? 22 : 44; height: width
        sourceSize { width: width; height: height }
        anchors.centerIn: parent
        source: imageSource
    }
}
