//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceTrigon.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

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
        "image://pentobi/triangle/" + color[0] + "/" + color[1] + "/"
        + color[2]
    property string imageNameDownward:
        "image://pentobi/triangle-down/" + color[0] + "/" + color[1] + "/"
        + color[2]
    property real scaleUnplayed:
        parentUnplayed ? 0.14 * parentUnplayed.width / board.gridWidth : 0
    property bool flippedX: Math.abs(flipX.angle - 180) < 90
    property real pieceAngle: flippedX ? rotation + 180 : rotation
    property real isSmall: scale < 0.5 ? 1 : 0
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
                property bool flippedY: Math.abs(flipY.angle - 180) < 90

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
    StateGroup {
        state: pieceModel.state

        states: [
            State {
                name: "60"

                PropertyChanges { target: root; rotation: 60 }
                // See comment in PieceClassic about flipY property change
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "120"

                PropertyChanges { target: root; rotation: 120 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "180"

                PropertyChanges { target: root; rotation: 180 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "240"

                PropertyChanges { target: root; rotation: 240 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "300"

                PropertyChanges { target: root; rotation: 300 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "flip"

                PropertyChanges { target: flipX; angle: 180 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "60flip"

                PropertyChanges { target: root; rotation: 60 }
                PropertyChanges { target: flipX; angle: 180 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "120flip"

                PropertyChanges { target: root; rotation: 120 }
                PropertyChanges { target: flipX; angle: 180 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "180flip"

                PropertyChanges { target: root; rotation: 180 }
                PropertyChanges { target: flipX; angle: 180 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "240flip"

                PropertyChanges { target: root; rotation: 240 }
                PropertyChanges { target: flipX; angle: 180 }
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "300flip"

                PropertyChanges { target: root; rotation: 300 }
                PropertyChanges { target: flipX; angle: 180 }
                PropertyChanges { target: flipY; angle: 0 }
            }
        ]

        transitions: [
            Transition {
                from: ",180flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "60,240flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "120,300flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "180,flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "240,60flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "300,120flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                enabled: enableAnimations

                PieceRotationAnimation { }
                PieceRotationAnimation { target: flipX; property: "angle" }
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
                via: isDesktop ? null : gameView

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
