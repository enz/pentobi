//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceElementClassic.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

// Piece element (square) with pseudo-3D effect.
// Simulates lighting by using different images for the lighting at different
// rotations and interpolating between them with an opacity animation.
Item {
    id: root

    Repeater {
        // Image rotation
        model: [ 0, 90, 180, 270 ]

        Item {
            property real _imageOpacity: {
                var angle = pieceAngle - modelData
                angle = ((angle % 360) + 360) % 360 // JS modulo bug
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
                    rotation: -modelData
                }
            }
        }
    }
}
