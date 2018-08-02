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
        function loadImage() {
            if (opacity > 0 && ! item)
                sourceComponent = component0
        }

        opacity: imageOpacity0
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component0

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
            }
        }
    }
    Loader {
        function loadImage() {
            if (opacity > 0 && ! item)
                sourceComponent = component90
        }

        opacity: imageOpacity90
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

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
        function loadImage() {
            if (opacity > 0 && ! item)
                sourceComponent = component180
        }

        opacity: imageOpacity180
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

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
        function loadImage() {
            if (opacity > 0 && ! item)
                sourceComponent = component270
        }

        opacity: imageOpacity270
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

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
