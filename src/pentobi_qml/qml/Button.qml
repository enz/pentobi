import QtQuick 2.0
import QtQuick.Controls 1.1

/** Button that supports an automatically scaled image.
    The image source should be a SVG file with size 22x22. */
ToolButton {
    id: root

    property string imageSource

    Image {
        // Prefer scale factor 1 or 2 unless image is very small or very large
        height: root.height < 26 || root.height > 100 ?
                    0.7 * root.height : root.height < 50 ? 22 : 44
        width: height
        sourceSize.height: height
        sourceSize.width: width
        anchors.centerIn: parent
        source: imageSource
        opacity: root.enabled ? 1 : 0.4
    }
}
