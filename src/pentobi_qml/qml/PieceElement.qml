//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

/* Piece element (square or triangle) with pseudo-3D border.
   Simulates a 3D border with a fixed light source by using different images
   for the border at different rotations and interpolating between them with an
   opacity animation. */
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

    // Image of the piece element without border
    Image {
        width: root.width
        height: root.height
        source: {
            if (isDownward)
                return "images/triangle-down-" + colorName + ".svg"
            else if (isTrigon)
                return "images/triangle-" + colorName + ".svg"
            else
                return "images/square-" + colorName + ".svg"
        }
        sourceSize { width: imageSourceWidth; height: imageSourceHeight }
    }

    // Border images
    Repeater {
        // Image rotation
        model: isTrigon ? [ 0, 60, 120, 180, 240, 300 ] : [ 0, 90, 180, 270 ]

        Image {
            z: 1
            source: {
                if (isDownward)
                    return "images/triangle-down-border-" + modelData + ".svg"
                else if (isTrigon)
                    return "images/triangle-border-" + modelData + ".svg"
                else
                    return "images/square-border-" + modelData + ".svg"
            }
            width: isTrigon ? 2 * gridElementWidth : gridElementWidth
            height: gridElementHeight
            sourceSize { width: imageSourceWidth; height: imageSourceHeight }
            opacity: {
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
                return 0.15 * opacity
            }
        }
    }
}
