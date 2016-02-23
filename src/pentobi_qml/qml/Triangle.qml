import QtQuick 2.3

// See Square.qml for comments
Item {
    id: root

    property bool isDownward

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
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                opacity: imageOpacity
                mipmap: true
                antialiasing: true
            }
        }
    }
    Loader {
        property real imageOpacity: imageOpacity60

        function loadImage() {
            if (imageOpacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component60
            imageOpacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        onImageOpacityChanged: loadImage()

        Component {
            id: component60

            Image {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                transform: [
                    Rotation {
                        angle: -60
                        origin {
                            x: width / 2
                            y: isDownward ? 2 * height / 3 : height / 3
                        }
                    },
                    Translate { y:  isDownward ? -height / 3 : height / 3 }
                ]
            }
        }
    }
    Loader {
        property real imageOpacity: imageOpacity120

        function loadImage() {
            if (imageOpacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component120
            imageOpacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        onImageOpacityChanged: loadImage()

        Component {
            id: component120

            Image {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                transform: Rotation {
                    angle: -120
                    origin {
                        x: width / 2
                        y: isDownward ? height / 3 : 2 * height / 3
                    }
                }
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
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                transform: [
                    Rotation {
                        angle: -180
                        origin {
                            x: width / 2
                            y: isDownward ? 2 * height / 3 : height / 3
                        }
                    },
                    Translate { y:  isDownward ? -height / 3 : height / 3 }
                ]
            }
        }
    }
    Loader {
        property real imageOpacity: imageOpacity240

        function loadImage() {
            if (imageOpacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component240
            imageOpacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        onImageOpacityChanged: loadImage()

        Component {
            id: component240

            Image {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                transform: Rotation {
                    angle: -240
                    origin {
                        x: width / 2
                        y: isDownward ? height / 3 : 2 * height / 3
                    }
                }
            }
        }
    }
    Loader {
        property real imageOpacity: imageOpacity300

        function loadImage() {
            if (imageOpacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component300
            imageOpacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        onImageOpacityChanged: loadImage()

        Component {
            id: component300

            Image {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
                opacity: imageOpacity
                transform: [
                    Rotation {
                        angle: -300
                        origin {
                            x: width / 2
                            y: isDownward ? 2 * height / 3 : height / 3
                        }
                    },
                    Translate { y:  isDownward ? -height / 3 : height / 3 }
                ]
            }
        }
    }
}
