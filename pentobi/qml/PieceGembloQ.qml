//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceGembloQ.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

PieceBase
{
    id: root

    property string imageName:
        "image://pentobi/quarter-square/" + color[0] + "/" + color[2]
    property string imageNameBottom:
        "image://pentobi/quarter-square/" + color[0] + "/" + color[1]
    // Avoid fractional sizes for square piece elements
    property real scaleUnplayed:
        parentUnplayed ? Math.floor(0.08 * 2 * parentUnplayed.width)
                         / (2 * board.gridWidth) : 0
    property real pieceAngle: flippedX ? rotation + 180 : rotation
    property real imageOpacity0: imageOpacity(pieceAngle, 0) * (1 - isSmall)
    property real imageOpacity90: imageOpacity(pieceAngle, 90) * (1 - isSmall)
    property real imageOpacity180: imageOpacity(pieceAngle, 180) * (1 - isSmall)
    property real imageOpacity270: imageOpacity(pieceAngle, 270) * (1 - isSmall)
    property real imageOpacitySmall0: imageOpacity(pieceAngle, 0) * isSmall
    property real imageOpacitySmall90: imageOpacity(pieceAngle, 90) * isSmall
    property real imageOpacitySmall180: imageOpacity(pieceAngle, 180) * isSmall
    property real imageOpacitySmall270: imageOpacity(pieceAngle, 270) * isSmall

    function imageOpacity(pieceAngle, imgAngle) {
        // Don't use the faster (pieceAngle - imgAngle + 360) % 360 here. The
        // effective angle can temporarily become negative for all images
        // during transitions if the rotate backward button is quickly hit
        // multiple times, and unlike in the other game variants, the opacity
        // function here returns 0 on negative angles, making the piece
        // temporarily disappear.
        var angle = ((pieceAngle - imgAngle) % 360 + 360) % 360
        if (angle <= 90) return 0
        if (angle <= 180) return -Math.cos(angle * Math.PI / 180)
        if (angle <= 270) {
            var o = -Math.cos(angle * Math.PI / 180)
            return o + (1 - o) * (-Math.sin(angle * Math.PI / 180))
        }
        return -Math.sin(angle * Math.PI / 180)
    }

    Repeater {
        model: pieceModel.elements

        QuarterSquare {
            x: (modelData.x - pieceModel.center.x) * board.gridWidth
            y: (modelData.y - pieceModel.center.y) * board.gridHeight
            pointType: {
                var t = modelData.x
                if (modelData.y % 2 != 0) t += 2
                return (t % 4 + 4) % 4
            }
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
                width: 0.45 * board.gridHeight
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
                width: 2 * board.gridWidth
                height: 2 * board.gridHeight
                fontSizeMode: Text.Fit
                font {
                    pixelSize: 0.7 * board.gridHeight
                    preferShaping: false
                }
                minimumPixelSize: 5
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                x: pieceModel.labelPos.x * board.gridWidth - width / 2
                y: pieceModel.labelPos.y * board.gridHeight - height / 2
                transform: [
                    Rotation {
                        origin { x: board.gridWidth; y: board.gridHeight }
                        axis { x: 0; y: 1; z: 0 }
                        angle: flippedY ? -180 : 0
                    },
                    Rotation {
                        origin { x: board.gridWidth; y: board.gridHeight }
                        axis { x: 1; y: 0; z: 0 }
                        angle: flippedX ? -180 : 0
                    },
                    Rotation {
                        origin { x: board.gridWidth; y: board.gridHeight }
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
