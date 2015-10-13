import QtQuick 2.0

// See Square.qml for comments
Item {
    id: root

    property bool isDownward

    Loader {
        property real imageOpacity: imageOpacity0

        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component0

        Component {
            id: component0

            Image {
                source: isDownward ? imageNameDownward : imageName
                width: root.width
                height: root.height
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                opacity: imageOpacity
                antialiasing: true
            }
        }
    }
    Loader {
        property real imageOpacity: imageOpacity60

        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component60

        Component {
            id: component60

            Image {
                source: isDownward ? imageName : imageNameDownward
                width: root.width
                height: root.height
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                opacity: imageOpacity
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
        property real imageOpacity: imageOpacity120

        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component120

        Component {
            id: component120

            Image {
                source: isDownward ? imageNameDownward : imageName
                width: root.width
                height: root.height
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                opacity: imageOpacity
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
        property real imageOpacity: imageOpacity180

        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component180

        Component {
            id: component180

            Image {
                source: isDownward ? imageName : imageNameDownward
                width: root.width
                height: root.height
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                opacity: imageOpacity
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
        property real imageOpacity: imageOpacity240

        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component240

        Component {
            id: component240

            Image {
                source: isDownward ? imageNameDownward : imageName
                width: root.width
                height: root.height
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                opacity: imageOpacity
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
        property real imageOpacity: imageOpacity300

        onImageOpacityChanged:
            if (imageOpacity > 0 && status === Loader.Null)
                sourceComponent = component300

        Component {
            id: component300

            Image {
                source: isDownward ? imageName : imageNameDownward
                width: root.width
                height: root.height
                sourceSize {
                    width: imageSourceWidth
                    height: imageSourceHeight
                }
                asynchronous: true
                opacity: imageOpacity
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
