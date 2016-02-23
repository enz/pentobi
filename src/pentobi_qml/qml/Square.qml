import QtQuick 2.3

// Piece element (square) with pseudo-3D effect.
// Simulates lighting by using different images for the lighting at different
// rotations and interpolating between them with an opacity animation.
Item {
    id: root

    Loader {
        property real imageOpacity: imageOpacity0

        function loadImage() {
            if (imageOpacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component0
            imageOpacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        onImageOpacityChanged: loadImage()

        Component {
            id: component0

            Image {
                source: imageName
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
            }
        }
    }
    Loader {
        property real imageOpacity: imageOpacity90

        function loadImage() {
            if (imageOpacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component90
            imageOpacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        onImageOpacityChanged: loadImage()

        Component {
            id: component90

            Image {
                source: imageName
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                rotation: -90
            }
        }
    }
    Loader {
        property real imageOpacity: imageOpacity180

        function loadImage() {
            if (imageOpacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component180
            imageOpacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        onImageOpacityChanged: loadImage()

        Component {
            id: component180

            Image {
                source: imageName
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                rotation: -180
            }
        }
    }
    Loader {
        property real imageOpacity: imageOpacity270

        function loadImage() {
            if (imageOpacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component270
            imageOpacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        onImageOpacityChanged: loadImage()

        Component {
            id: component270

            Image {
                source: imageName
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                rotation: -270
            }
        }
    }
}
