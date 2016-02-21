import QtQuick 2.3

// Piece element (square) with pseudo-3D effect.
// Simulates lighting by using different images for the lighting at different
// rotations and interpolating between them with an opacity animation.
Item {
    id: root

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
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                mipmap: true
                antialiasing: true
                asynchronous: true
                opacity: imageOpacity
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
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                rotation: -90
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
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                rotation: -180
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
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                rotation: -270
            }
        }
    }
}
