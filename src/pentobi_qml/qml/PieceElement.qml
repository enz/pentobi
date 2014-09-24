//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

/* Piece element (square or triangle) with pseudo-3D border.
   Simulates a 3D border with a fixed light source by using different images
   for the lighting at different rotations and interpolating between them with
   an opacity animation. */
Item {
    id: root

    property bool isTrigon
    // Only used in Trigon to determine if triangle is upside-down.
    property bool isDownward
    property string colorName
    property real gridElementWidth
    property real gridElementHeight
    property real imageSourceWidth
    property real imageSourceHeight

    // Lighting transformations applied in this order:
    property real angle: 0     // Rotate lighting around z axis
    property bool flipX: false // Flip lighting around x axis
    property bool flipY: false // Flip lighting around y axis

    width: isTrigon ? 2 * gridElementWidth : gridElementWidth
    height: gridElementHeight

    Repeater {
        // Image rotation
        model: isTrigon ? [ 0, 60, 120, 180, 240, 300 ] : [ 0, 90, 180, 270 ]

        Item {
            property real _imageOpacity: {
                var imgAngle = angle - modelData
                var opacity
                if (isTrigon) {
                    if (flipX && flipY) imgAngle += 180
                    else if (flipX) imgAngle += 60
                    else if (flipY) imgAngle += 240
                    opacity = 2 * Math.cos(imgAngle * Math.PI / 180) - 1
                } else {
                    if (flipX && flipY) imgAngle += 180
                    else if (flipX) imgAngle += 90
                    else if (flipY) imgAngle += 270
                    opacity = Math.cos(imgAngle * Math.PI / 180)
                }
                if (opacity < 0.1) opacity = 0
                return opacity
            }
            on_ImageOpacityChanged:
                if (_imageOpacity > 0 && loader.status == Loader.Null)
                    loader.sourceComponent = component

            Loader { id: loader }
            Component {
                id: component
                Image {
                    source: {
                        var prefix
                        if (isDownward)
                            prefix = "triangle-down-"
                        else if (isTrigon)
                            prefix = "triangle-"
                        else
                            prefix = "square-"
                        return theme.getImage(prefix + colorName + "-" + modelData)
                    }
                    width: isTrigon ? 2 * gridElementWidth : gridElementWidth
                    height: gridElementHeight
                    sourceSize { width: imageSourceWidth; height: imageSourceHeight }
                    opacity: _imageOpacity
                }
            }
        }
    }
}
