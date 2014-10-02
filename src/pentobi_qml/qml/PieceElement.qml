//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

// Piece element (square or triangle) with pseudo-3D effect.
// Simulates lighting by using different images for the lighting at different
// rotations and interpolating between them with an opacity animation.
Item {
    id: root

    property bool isTrigon
    property bool isDownward
    property string imageName
    property string imageNameDownward
    property real angle
    property bool fastRendering

    Repeater {
        // Image rotation
        model: isTrigon ? [ 0, 60, 120, 180, 240, 300 ] : [ 0, 90, 180, 270 ]

        Item {
            property real _imageOpacity: {
                var angle = Math.round(root.angle - modelData)
                angle = ((angle % 360) + 360) % 360 // JS modulo bug
                if (isTrigon) {
                    if (angle >= 60 && angle <= 300) return 0
                    return 2 * Math.cos(angle * Math.PI / 180) - 1
                } else {
                    if (angle >= 90 && angle <= 270) return 0
                    return Math.cos(angle * Math.PI / 180)
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
                    opacity: _imageOpacity
                    smooth: ! fastRendering
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
