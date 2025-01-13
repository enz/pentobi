//-----------------------------------------------------------------------------
/** @file pentobi/PieceTrigon.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

PieceBase
{
    id: root

    property string imageName:
        "image://pentobi/triangle/" + color[0] + "/" + color[1] + "/"
        + color[2]
    property string imageNameDownward:
        "image://pentobi/triangle-down/" + color[0] + "/" + color[1] + "/"
        + color[2]
    property real scaleUnplayed:
        parentUnplayed ? 0.14 * parentUnplayed.width / board.gridWidth : 0
    property real imageOpacity0: imageOpacity(pieceAngle, 0) * (1 - isSmall)
    property real imageOpacity60: imageOpacity(pieceAngle, 60) * (1 - isSmall)
    property real imageOpacity120: imageOpacity(pieceAngle, 120) * (1 - isSmall)
    property real imageOpacity180: imageOpacity(pieceAngle, 180) * (1 - isSmall)
    property real imageOpacity240: imageOpacity(pieceAngle, 240) * (1 - isSmall)
    property real imageOpacity300: imageOpacity(pieceAngle, 300) * (1 - isSmall)
    property real imageOpacitySmall0: imageOpacity(pieceAngle, 0) * isSmall
    property real imageOpacitySmall60: imageOpacity(pieceAngle, 60) * isSmall
    property real imageOpacitySmall120: imageOpacity(pieceAngle, 120) * isSmall
    property real imageOpacitySmall180: imageOpacity(pieceAngle, 180) * isSmall
    property real imageOpacitySmall240: imageOpacity(pieceAngle, 240) * isSmall
    property real imageOpacitySmall300: imageOpacity(pieceAngle, 300) * isSmall

    function _isDownward(pos) { return (pos.x % 2 == 0) != (pos.y % 2 == 0) }
    function imageOpacity(pieceAngle, imgAngle) {
        var angle = (pieceAngle - imgAngle + 360) % 360
        return angle >= 60 && angle <= 300 ? 0 : 2 * Math.cos(angle * Math.PI / 180) - 1
    }

    Repeater {
        model: pieceModel.elements

        Triangle {
            isDownward: _isDownward(modelData)
            width: 2 * board.gridWidth
            height: board.gridHeight
            x: (modelData.x - pieceModel.center.x - 0.5) * board.gridWidth
            y: (modelData.y - pieceModel.center.y) * board.gridHeight
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
                font.pixelSize: 0.5 * board.gridHeight
                minimumPixelSize: 5
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
