import QtQuick 2.3

// Piece element for Nexos. See Square.qml for comments.
Item {
    id: root

    property bool isHorizontal

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
                mirror: ! isHorizontal
                rotation: isHorizontal ? 0 : -90
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
                mirror: isHorizontal
                rotation: isHorizontal ? -180 : -90
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
                mirror: ! isHorizontal
                rotation: isHorizontal ? -180 : -270
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
                mirror: isHorizontal
                rotation: isHorizontal ? 0 : -270
            }
        }
    }
}
