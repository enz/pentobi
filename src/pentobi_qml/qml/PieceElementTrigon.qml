import QtQuick 2.0

// See PieceElementClassic.qml for comments
Item {
    id: root

    property bool isDownward

    Repeater {
        model: [ 0, 120, 240 ]

        Item {
            property real _imageOpacity: {
                var angle = pieceAngle - modelData
                angle = ((angle % 360) + 360) % 360
                if (isDownward) {
                    if (angle >= 300) return 1
                    if (angle > 240)
                        return 2 * Math.cos((angle + 60) * Math.PI / 180) - 1
                    if (angle >= 60) return 0
                    return 2 * Math.cos(angle * Math.PI / 180) - 1
                }
                if (angle <= 60) return 1
                if (angle < 120)
                    return 2 * Math.cos((angle - 60) * Math.PI / 180) - 1
                if (angle <= 300) return 0
                return 2 * Math.cos(angle * Math.PI / 180) - 1
            }

            on_ImageOpacityChanged:
                if (_imageOpacity > 0 && loader.status == Loader.Null)
                    loader.sourceComponent = component

            Loader { id: loader }
            Component {
                id: component
                Image {
                    source: imageName
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
                            angle: isDownward ? -modelData + 60 : -modelData
                            origin {
                                x: width / 2
                                y: 2 * height / 3
                            }
                        },
                        Translate { y: isDownward ? -height / 3 : 0 }
                    ]
                }
            }
        }
    }
}
