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

    property string _imageName:
        theme.getImage((isTrigon ? "triangle-" : "square-") + colorName)
    property string _imageNameDownward:
        theme.getImage("triangle-down-" + colorName)

    Repeater {
        // Image rotation
        model: isTrigon ? [ 0, 60, 120, 180, 240, 300 ] : [ 0, 90, 180, 270 ]

        Item {
            property real _imageOpacity: {
                var imgAngle = Math.round(root.angle - modelData)
                if (isTrigon) {
                    if (flipX) {
                        if (flipY) imgAngle += 180
                        else imgAngle += 120
                    }
                    else if (flipY) imgAngle += 240
                    imgAngle = ((imgAngle % 360) + 360) % 360 // JS modulo bug
                    if (imgAngle >= 60 && imgAngle <= 300) return 0
                    return 2 * Math.cos(imgAngle * Math.PI / 180) - 1
                } else {
                    if (flipX) {
                        if (flipY) imgAngle += 180
                        else imgAngle += 90
                    }
                    else if (flipY) imgAngle += 270
                    imgAngle = ((imgAngle % 360) + 360) % 360
                    if (imgAngle >= 90 && imgAngle <= 270) return 0
                    return Math.cos(imgAngle * Math.PI / 180)
                }
            }

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

                    source: _isImageDownward ? _imageNameDownward : _imageName
                    width: root.width
                    height: root.height
                    sourceSize {
                        width: isTrigon ? 2 * imageSourceWidth :
                                          imageSourceWidth
                        height: imageSourceHeight
                    }
                    smooth: ! fastRendering
                    opacity: _imageOpacity
                    transform: [
                        Rotation {
                            angle: -modelData
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
}
