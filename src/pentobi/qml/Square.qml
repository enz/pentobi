//-----------------------------------------------------------------------------
/** @file pentobi/qml/Square.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// Piece element (square) with pseudo-3D effect.
// Simulates lighting by using different images for the lighting at different
// rotations and interpolating between them with an opacity animation.
Item {
    Loader {
        opacity: imageOpacity0
        sourceComponent: opacity > 0 || item ? component0 : null

        Component {
            id: component0

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
            }
        }
    }
    Loader {
        opacity: imageOpacity90
        sourceComponent: opacity > 0 || item ? component90 : null

        Component {
            id: component90

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
                rotation: -90
            }
        }
    }
    Loader {
        opacity: imageOpacity180
        sourceComponent: opacity > 0 || item ? component180 : null

        Component {
            id: component180

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
                rotation: -180
            }
        }
    }
    Loader {
        opacity: imageOpacity270
        sourceComponent: opacity > 0 || item ? component270 : null

        Component {
            id: component270

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
                rotation: -270
            }
        }
    }
}
