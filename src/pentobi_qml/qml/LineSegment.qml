import QtQuick 2.3

// Piece element for Nexos. See Square.qml for comments.
Item {
    id: root

    property bool isHorizontal

    Loader {
        property real imageOpacity: imageOpacity0

        anchors.fill: root
        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component0

        Component {
            id: component0

            Image {
                source: imageName
                sourceSize: imageSourceSize
                asynchronous: true
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

        anchors.fill: root
        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component90

        Component {
            id: component90

            Image {
                source: imageName
                sourceSize: imageSourceSize
                asynchronous: true
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

        anchors.fill: root
        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component180

        Component {
            id: component180

            Image {
                source: imageName
                sourceSize: imageSourceSize
                asynchronous: true
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

        anchors.fill: root
        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component270

        Component {
            id: component270

            Image {
                source: imageName
                sourceSize: imageSourceSize
                asynchronous: true
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                mirror: isHorizontal
                rotation: isHorizontal ? 0 : -270
            }
        }
    }
}
