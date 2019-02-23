//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceCallisto.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3
import QtQuick.Window 2.2

// See PieceClassic.qml for comments
Item
{
    id: root

    property QtObject pieceModel
    property var color:
        switch (pieceModel.color) {
        case 0: return color0
        case 1: return color1
        case 2: return color2
        case 3: return color3
        }
    property Item parentUnplayed
    property string imageName:
        "image://pentobi/" +
        (pieceModel.elements.length === 1 ? "frame" : "square") +
        "/" + color[0] + "/" + color[1] + "/" + color[2]
    // Avoid fractional sizes for square piece elements
    property real scaleUnplayed:
        parentUnplayed ?
            Math.floor(0.25 * parentUnplayed.width) / board.gridWidth : 0
    // We  only use flipX.angle [0..360]
    property bool flippedX: Math.abs(flipX.angle - 180) < 90
    // We  only use flipY.angle [0..180]
    property bool flippedY: flipY.angle > 90
    property real pieceAngle: {
        if (! flippedY && ! flippedX) return rotation
        if (! flippedY && flippedX) return rotation + 90
        if (flippedX) return rotation + 180
        return rotation + 270
    }
    property real isSmall: scale < 0.5 ? 1 : 0
    property real imageOpacity0: imageOpacity(pieceAngle, 0) * (1 - isSmall)
    property real imageOpacity90: imageOpacity(pieceAngle, 90) * (1 - isSmall)
    property real imageOpacity180: imageOpacity(pieceAngle, 180) * (1 - isSmall)
    property real imageOpacity270: imageOpacity(pieceAngle, 270) * (1 - isSmall)
    property real imageOpacitySmall0: imageOpacity(pieceAngle, 0) * isSmall
    property real imageOpacitySmall90: imageOpacity(pieceAngle, 90) * isSmall
    property real imageOpacitySmall180: imageOpacity(pieceAngle, 180) * isSmall
    property real imageOpacitySmall270: imageOpacity(pieceAngle, 270) * isSmall

    transform: [
        Rotation {
            id: flipX

            axis { x: 1; y: 0; z: 0 }
        },
        Rotation {
            id: flipY

            axis { x: 0; y: 1; z: 0 }
        }
    ]

    function imageOpacity(pieceAngle, imgAngle) {
        var angle = (pieceAngle - imgAngle + 360) % 360
        return angle >= 90 && angle <= 270 ? 0 : Math.cos(angle * Math.PI / 180)
    }

    Repeater {
        model: pieceModel.elements

        Item {
            // Right junction
            Rectangle {
                visible: pieceModel.junctionType[index] === 0
                         || pieceModel.junctionType[index] === 1
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
                visible: pieceModel.junctionType[index] === 0
                         || pieceModel.junctionType[index] === 2
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
                visible: pieceModel.junctionType[index] === 0
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
    Rectangle {
        opacity: moveMarking == "last_dot" && pieceModel.isLastMove ? 0.5 : 0
        color: gameModel.showVariations && ! gameModel.isMainVar ? "transparent" : border.color
        border { width: 0.2 * width; color: root.color[3] }
        width: 0.3 * board.gridHeight
        height: width
        radius: width / 2
        x: pieceModel.labelPos.x * board.gridWidth - width / 2
        y: pieceModel.labelPos.y * board.gridHeight - height / 2
        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
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
    StateGroup {
        state: pieceModel.state

        states: [
            State {
                name: "rot90"
                PropertyChanges { target: root; rotation: 90 }
            },
            State {
                name: "rot180"
                PropertyChanges { target: root; rotation: 180 }
            },
            State {
                name: "rot270"
                PropertyChanges { target: root; rotation: 270 }
            },
            State {
                name: "flip"
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot90Flip"
                PropertyChanges { target: root; rotation: 90 }
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot180Flip"
                PropertyChanges { target: root; rotation: 180 }
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot270Flip"
                PropertyChanges { target: root; rotation: 270 }
                PropertyChanges { target: flipX; angle: 180 }
            }
        ]

        transitions: [
            Transition {
                from: ",rot180Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot90,rot270Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot180,flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot270,rot90Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                enabled: enableAnimations

                PieceRotationAnimation { }
                PieceFlipAnimation { target: flipX }
            }
        ]
    }

    states: [
        State {
            name: "picked"
            when: root === pickedPiece

            ParentChange {
                target: root
                parent: pieceManipulator
                x: pieceManipulator.width / 2
                y: pieceManipulator.height / 2
            }
        },
        State {
            name: "played"
            when: pieceModel.isPlayed

            ParentChange {
                target: root
                parent: board.grabImageTarget
                x: board.mapFromGameX(pieceModel.gameCoord.x) - board.grabImageTarget.x
                y: board.mapFromGameY(pieceModel.gameCoord.y) - board.grabImageTarget.y
            }
        },
        State {
            name: "unplayed"
            when: parentUnplayed != null

            PropertyChanges {
                target: root
                scale: scaleUnplayed
            }
            ParentChange {
                target: root
                parent: parentUnplayed
                x: parentUnplayed.width / 2
                y: parentUnplayed.height / 2
            }
        }
    ]
    transitions:
        Transition {
        from: "unplayed,picked,played"; to: from
        enabled: enableAnimations

        SequentialAnimation {
            PropertyAction {
                target: parentUnplayed.parent
                property: "z"; value: 1
            }
            ParentAnimation {
                via: isDesktop ? null : gameDisplay

                NumberAnimation {
                    properties: "x,y,scale"
                    duration: animationDurationMove
                    easing.type: Easing.InOutSine
                }
            }
            PropertyAction {
                target: parentUnplayed.parent
                property: "z"; value: 0
            }
        }
    }
}
