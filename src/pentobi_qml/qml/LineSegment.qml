import QtQuick 2.3

// Piece element for Nexos. See Square.qml for comments.
Item {
    id: root

    property bool isHorizontal

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
                antialiasing: true
                mirror: ! isHorizontal
                rotation: isHorizontal ? 0 : -90
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
                antialiasing: true
                mirror: isHorizontal
                rotation: isHorizontal ? -180 : -90
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
                antialiasing: true
                mirror: ! isHorizontal
                rotation: isHorizontal ? -180 : -270
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
                antialiasing: true
                mirror: isHorizontal
                rotation: isHorizontal ? 0 : -270
            }
        }
    }
}
