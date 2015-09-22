import QtQuick 2.0

// See Square.qml for comments
Item {
    id: root

    property bool isDownward

    Repeater {
        model: [ 0, 60, 120, 180, 240, 300 ]

        Loader {
            property real _imageOpacity: {
                if (pieceAngle == 0) return modelData == 0 ? 1 : 0
                var angle = (((pieceAngle - modelData) % 360) + 360) % 360
                return (angle >= 60 && angle <= 300 ? 0 : 2 * Math.cos(angle * Math.PI / 180) - 1)
            }

            on_ImageOpacityChanged:
                if (_imageOpacity > 0 && status == Loader.Null)
                    sourceComponent = component

            Component {
                id: component

                Image {
                    property bool _switchUpDownImage: modelData % 120 != 0
                    property bool _isImageDownward:
                        (isDownward != _switchUpDownImage)

                    source: _isImageDownward ? imageNameDownward : imageName
                    width: root.width
                    height: root.height
                    sourceSize {
                        width: imageSourceWidth
                        height: imageSourceHeight
                    }
                    asynchronous: true
                    opacity: _imageOpacity
                    antialiasing: true
                    transform: [
                        Rotation {
                            angle: -modelData
                            origin {
                                x: width / 2
                                y: _isImageDownward ?
                                       height / 3 : 2 * height / 3
                            }
                        },
                        Translate {
                            y: {
                                if (! isDownward && _isImageDownward)
                                    return height / 3
                                if (isDownward && ! _isImageDownward)
                                    return -height / 3
                                return 0
                            }
                        }
                    ]
                }
            }
        }
    }
}
