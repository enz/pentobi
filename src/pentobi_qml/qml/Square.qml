import QtQuick 2.3

// Piece element (square) with pseudo-3D effect.
// Simulates lighting by using different images for the lighting at different
// rotations and interpolating between them with an opacity animation.
Item {
    id: root

    Loader {
        function loadImage() {
            if (opacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component0
            opacityChanged.disconnect(loadImage)
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
                antialiasing: true
            }
        }
    }
    Loader {
        function loadImage() {
            if (opacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component90
            opacityChanged.disconnect(loadImage)
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
                antialiasing: true
                rotation: -90
            }
        }
    }
    Loader {
        function loadImage() {
            if (opacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component180
            opacityChanged.disconnect(loadImage)
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
                antialiasing: true
                rotation: -180
            }
        }
    }
    Loader {
        function loadImage() {
            if (opacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component270
            opacityChanged.disconnect(loadImage)
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
                antialiasing: true
                rotation: -270
            }
        }
    }
}
