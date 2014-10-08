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

    property bool isDownward

    Repeater {
        // Image rotation
        model: isTrigon ? [ 0, 120, 240 ] : [ 0, 90, 180, 270 ]

        Item {
            property real _imageOpacity: {
                var angle = Math.round(pieceAngle - modelData)
                angle = ((angle % 360) + 360) % 360 // JS modulo bug
                if (isDownward) {
                    if (angle >= 300) return 1
                    if (angle > 240)
                        return 2 * Math.cos((angle + 60) * Math.PI / 180) - 1
                    if (angle >= 60) return 0
                    return 2 * Math.cos(angle * Math.PI / 180) - 1
                }
                if (isTrigon) {
                    if (angle <= 60) return 1
                    if (angle < 120)
                        return 2 * Math.cos((angle - 60) * Math.PI / 180) - 1
                    if (angle <= 300) return 0
                    return 2 * Math.cos(angle * Math.PI / 180) - 1
                }
                if (angle >= 90 && angle <= 270) return 0
                return Math.cos(angle * Math.PI / 180)
            }

            on_ImageOpacityChanged:
                if (_imageOpacity > 0 && loader.status == Loader.Null)
                    loader.sourceComponent = component

            Loader { id: loader }
            Component {
                id: component
                Image {
                    source: imageName
                    width: root.width
                    height: root.height
                    sourceSize {
                        width: imageSourceWidth
                        height: imageSourceHeight
                    }
                    opacity: _imageOpacity
                    smooth: ! fastRendering
                    transform: [
                        Rotation {
                            angle: isDownward ? -modelData + 60 : -modelData
                            origin {
                                x: width / 2
                                y: isTrigon ? 2 * height / 3 : height / 2
                            }
                        },
                        Translate { y: isDownward ? -height / 3 : 0 }
                    ]
                }
            }
        }
    }
}
