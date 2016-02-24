import QtQuick 2.3

// See Square.qml for comments
Item {
    id: root

    property bool isDownward

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
                source: isDownward ? imageNameDownward : imageName
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
            sourceComponent = component60
            opacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        opacity: imageOpacity60
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component60

            Image {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
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
        function loadImage() {
            if (opacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component120
            opacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        opacity: imageOpacity120
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component120

            Image {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
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
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
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
        function loadImage() {
            if (opacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component240
            opacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        opacity: imageOpacity240
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component240

            Image {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
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
        function loadImage() {
            if (opacity === 0 || status !== Loader.Null)
                return
            sourceComponent = component300
            opacityChanged.disconnect(loadImage)
        }

        anchors.fill: root
        opacity: imageOpacity300
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: component300

            Image {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                mipmap: true
                antialiasing: true
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
