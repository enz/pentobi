//-----------------------------------------------------------------------------
/** @file pentobi/PieceCallisto.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Window

PieceBase
{
    id: root

    property string imageName:
        "image://pentobi/" +
        (pieceModel.elements.length === 1 ? "frame" : "square") +
        "/" + color[0] + "/" + color[1] + "/" + color[2]
    // Avoid fractional sizes for square piece elements
    property real scaleUnplayed:
        parentUnplayed ?
            Math.floor(0.25 * parentUnplayed.width) / board.gridWidth : 0
    property real imageOpacity0: imageOpacity(pieceAngle, 0) * (1 - isSmall)
    property real imageOpacity90: imageOpacity(pieceAngle, 90) * (1 - isSmall)
    property real imageOpacity180: imageOpacity(pieceAngle, 180) * (1 - isSmall)
    property real imageOpacity270: imageOpacity(pieceAngle, 270) * (1 - isSmall)
    property real imageOpacitySmall0: imageOpacity(pieceAngle, 0) * isSmall
    property real imageOpacitySmall90: imageOpacity(pieceAngle, 90) * isSmall
    property real imageOpacitySmall180: imageOpacity(pieceAngle, 180) * isSmall
    property real imageOpacitySmall270: imageOpacity(pieceAngle, 270) * isSmall

    function imageOpacity(pieceAngle, imgAngle) {
        var angle = (pieceAngle - imgAngle + 360) % 360
        return angle >= 90 && angle <= 270 ? 0 : Math.cos(angle * Math.PI / 180)
    }

    Repeater {
        model: pieceModel.elements

        Item {
            // Right junction
            Rectangle {
                visible: pieceModel.junctionType[index] & 1
                color: root.color[0]
                width: board.gridWidth - square.width
                height: 0.95 * board.gridHeight
                x: (modelData.x - pieceModel.center.x + 1) * board.gridWidth
                   - width / 2
                y: (modelData.y - pieceModel.center.y) * board.gridHeight
                   + (board.gridHeight - height) / 2
                antialiasing: true
            }
            // Down junction
            Rectangle {
                visible: pieceModel.junctionType[index] & 2
                color: root.color[0]
                width: 0.95 * board.gridWidth
                height: board.gridHeight - square.height
                x: (modelData.x - pieceModel.center.x) * board.gridWidth
                   + (board.gridWidth - width) / 2
                y: (modelData.y - pieceModel.center.y + 1) * board.gridHeight
                   - height / 2
                antialiasing: true
            }
            // Right-down junction
            Rectangle {
                visible: pieceModel.junctionType[index] === 7
                color: root.color[0]
                width: board.gridWidth - square.width
                height: board.gridHeight - square.height
                x: (modelData.x - pieceModel.center.x + 1) * board.gridWidth
                   - width / 2
                y: (modelData.y - pieceModel.center.y + 1) * board.gridHeight
                   - height / 2
                antialiasing: true
            }
            Square {
                id: square

                width: 0.95 * board.gridWidth
                height: 0.95 * board.gridHeight
                x: (modelData.x - pieceModel.center.x) * board.gridWidth
                   + (board.gridWidth - width) / 2
                y: (modelData.y - pieceModel.center.y) * board.gridHeight
                   + (board.gridHeight - height) / 2
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
                border {
                    width: 0.2 * width
                    color: pieceModel.elements.length > 1 ?
                               root.color[3] : theme.colorBoardMarkup
                }
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
                color: pieceModel.elements.length > 1 ?
                           root.color[3] : theme.colorBoardMarkup
                width: board.gridWidth
                height: board.gridHeight
                fontSizeMode: Text.Fit
                font.pixelSize: 0.5 * board.gridHeight
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                minimumPixelSize: 5
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
