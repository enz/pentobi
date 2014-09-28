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
    property bool isMarked

    // Disable the lighting effect (always use only the first image with
    // lighting not rotated) and set smooth rendering of the image to false.
    // This will speed up animations that are too fast for the user to notice
    // visual details anyway.
    property bool fastRendering

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

            smooth: ! fastRendering
            on_ImageOpacityChanged:
                if (_imageOpacity > 0 && loader.status == Loader.Null)
                    loader.sourceComponent = component

            Loader { id: loader }
            Component {
                id: component
                Image {
                    property bool _switchUpDownImage:
                        isTrigon && modelData % 120 != 0
                    property bool _isImageDownward:
                        isTrigon && (isDownward != _switchUpDownImage)

                    source: {
                        if (_isImageDownward)
                            return theme.getImage("triangle-down-" + colorName)
                        if (isTrigon)
                            return theme.getImage("triangle-" + colorName)
                        return theme.getImage("square-" + colorName)
                    }
                    width: isTrigon ? 2 * gridElementWidth : gridElementWidth
                    height: gridElementHeight
                    sourceSize {
                        width: isTrigon ? 2 * imageSourceWidth :
                                          imageSourceWidth
                        height: imageSourceHeight
                    }
                    opacity: _imageOpacity
                    transform: [
                        Rotation {
                            angle: _switchUpDownImage ? 360 - modelData: -modelData
                            origin {
                                x: width / 2
                                y: {
                                    if (_isImageDownward)
                                        return height / 3
                                    else if (isTrigon)
                                        return 2 * height / 3
                                    else
                                        return height / 2
                                }
                            }
                        },
                        Translate {
                            y: {
                                if (! isDownward && _isImageDownward)
                                    return height / 3
                                if (isDownward && ! _isImageDownward)
                                    return -height / 3
                                return 0
                            }
                        }
                    ]
                }
            }
        }
    }
    Rectangle {
        id: marker

        visible: isMarked
        opacity: 0.5
        color: colorName == "blue" || colorName == "red" ? "white" : "#333333"
        width: 0.3 * gridElementWidth
        height: width
        radius: width / 2
        x: gridElementWidth / 2 - width / 2
        y: gridElementHeight / 2 - height / 2
    }
}
