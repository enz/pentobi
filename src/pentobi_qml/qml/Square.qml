import QtQuick 2.3

// Piece element (square) with pseudo-3D effect.
// Simulates lighting by using different images for the lighting at different
// rotations and interpolating between them with an opacity animation.
Item {
    id: root

    Loader {
        function loadImage() {
            if (opacity > 0 && status === Loader.Null)
                sourceComponent = component0
        }

        anchors.fill: root
        opacity: imageOpacity0
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component0

            Image {
                source: imageName
                sourceSize: imageSourceSize
                mipmap: true
            }
        }
    }
    Loader {
        function loadImage() {
            if (opacity > 0 && status === Loader.Null)
                sourceComponent = component90
        }

        anchors.fill: root
        opacity: imageOpacity90
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component90

            Image {
                source: imageName
                sourceSize: imageSourceSize
                mipmap: true
                rotation: -90
            }
        }
    }
    Loader {
        function loadImage() {
            if (opacity > 0 && status === Loader.Null)
                sourceComponent = component180
        }

        anchors.fill: root
        opacity: imageOpacity180
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component180

            Image {
                source: imageName
                sourceSize: imageSourceSize
                mipmap: true
                rotation: -180
            }
        }
    }
    Loader {
        function loadImage() {
            if (opacity > 0 && status === Loader.Null)
                sourceComponent = component270
        }

        anchors.fill: root
        opacity: imageOpacity270
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component270

            Image {
                source: imageName
                sourceSize: imageSourceSize
                mipmap: true
                rotation: -270
            }
        }
    }
}
