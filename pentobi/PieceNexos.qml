//-----------------------------------------------------------------------------
/** @file pentobi/PieceNexos.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

PieceBase
{
    id: root

    property string imageName:
        "image://pentobi/square/" + color[0] + "/" + color[1] + "/" + color[2]
    // Avoid fractional sizes for square piece elements
    property real scaleUnplayed:
        parentUnplayed ?
            Math.floor(0.12 * parentUnplayed.width) / board.gridWidth : 0
    property real imageOpacity0: imageOpacity(pieceAngle, 0) * (1 - isSmall)
    property real imageOpacity90: imageOpacity(pieceAngle, 90) * (1 - isSmall)
    property real imageOpacity180: imageOpacity(pieceAngle, 180) * (1 - isSmall)
    property real imageOpacity270: imageOpacity(pieceAngle, 270) * (1 - isSmall)
    property real imageOpacitySmall0: imageOpacity(pieceAngle, 0) * isSmall
    property real imageOpacitySmall90: imageOpacity(pieceAngle, 90) * isSmall
    property real imageOpacitySmall180: imageOpacity(pieceAngle, 180) * isSmall
    property real imageOpacitySmall270: imageOpacity(pieceAngle, 270) * isSmall

    function isHorizontal(pos) { return pos.x % 2 != 0 }
    function imageOpacity(pieceAngle, imgAngle) {
        var angle = (pieceAngle - imgAngle + 360) % 360
        return angle >= 90 && angle <= 270 ? 0 : Math.cos(angle * Math.PI / 180)
    }

    Repeater {
        model: pieceModel.elements

        LineSegment {
            isHorizontal: root.isHorizontal(modelData)
            width: 1.5 * board.gridWidth
            height: 0.5 * board.gridHeight
            x: (modelData.x - pieceModel.center.x - 0.25) * board.gridWidth
            y: (modelData.y - pieceModel.center.y + 0.25) * board.gridHeight
        }
    }
    Repeater {
        model: pieceModel.junctions

        Image {
            source: {
                switch (pieceModel.junctionType[index]) {
                case 0:
                    return  "image://pentobi/junction-all/" + color[0]
                case 1:
                case 2:
                case 3:
                case 4:
                    return  "image://pentobi/junction-t/" + color[0]
                case 5:
                case 6:
                    return  "image://pentobi/junction-straight/" + color[0]
                default:
                    return  "image://pentobi/junction-right/" + color[0]
                }
            }
            rotation: {
                switch (pieceModel.junctionType[index]) {
                case 1:
                case 9:
                    return 270
                case 2:
                case 6:
                case 8:
                    return 90
                case 4:
                case 7:
                    return 180
                default:
                    return 0
                }
            }
            width: 0.5 * board.gridWidth
            height: 0.5 * board.gridHeight
            x: (modelData.x - pieceModel.center.x + 0.25) * board.gridWidth
            y: (modelData.y - pieceModel.center.y + 0.25) * board.gridHeight
            sourceSize {
                width: imageSourceSize.width / 3
                height: imageSourceSize.height
            }
            antialiasing: true
        }
    }
    Loader {
        sourceComponent:
            (moveMarking == "last_dot" && pieceModel.isLastMove) || item ?
                dotComponent : null

        Component {
            id: dotComponent

            Rectangle {
                opacity: moveMarking == "last_dot" && pieceModel.isLastMove ?
                             0.5 : 0
                color: gameModel.showVariations && ! gameModel.isMainVar ?
                           "transparent" : border.color
                border { width: 0.2 * width; color: root.color[3] }
                width: 0.3 * board.gridHeight
                height: width
                radius: width / 2
                x: pieceModel.labelPos.x * board.gridWidth - width / 2
                y: pieceModel.labelPos.y * board.gridHeight - height / 2
                Behavior on opacity {
                    NumberAnimation { duration: animationDurationFast }
                }
            }
        }
    }
    Loader {
        sourceComponent: moveMarking === "all_number"
                         || moveMarking === "last_number" || item ?
                             textComponent : null

        Component {
            id: textComponent

            Text {
                text: moveMarking == "all_number"
                      || (moveMarking == "last_number"
                          && pieceModel.isLastMove) ?
                          pieceModel.moveLabel : ""
                opacity: text === "" ? 0 : 1
                color: root.color[3]
                width: board.gridWidth
                height: board.gridHeight
                fontSizeMode: Text.Fit
                font {
                    pixelSize: 0.5 * board.gridHeight
                    preferShaping: false
                }
                minimumPixelSize: Math.max(3, 0.3 * board.gridHeight)
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                x: pieceModel.labelPos.x * board.gridWidth - width / 2
                y: pieceModel.labelPos.y * board.gridHeight - height / 2
                transform: [
                    Rotation {
                        origin {
                            x: board.gridWidth / 2; y: board.gridHeight / 2
                        }
                        axis { x: 0; y: 1; z: 0 }
                        angle: flippedY ? -180 : 0
                    },
                    Rotation {
                        origin {
                            x: board.gridWidth / 2; y: board.gridHeight / 2
                        }
                        axis { x: 1; y: 0; z: 0 }
                        angle: flippedX ? -180 : 0
                    },
                    Rotation {
                        origin {
                            x: board.gridWidth / 2; y: board.gridHeight / 2
                        }
                        angle: -root.rotation
                    }
                ]
                Behavior on opacity {
                    NumberAnimation { duration: animationDurationFast }
                }
            }
        }
    }
}
