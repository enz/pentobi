//-----------------------------------------------------------------------------
/** @file pentobi/PieceBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Item
{
    id: root

    property QtObject pieceModel
    property Item parentUnplayed
    property var color:
        switch (pieceModel.color) {
        case 0: return color0
        case 1: return color1
        case 2: return color2
        case 3: return color3
        }
    property bool flippedX: Math.abs(flipX.angle - 180) < 90
    property bool flippedY: flipY.angle > 90
    property real pieceAngle: {
        if (! flippedY && ! flippedX) return rotation
        if (! flippedY && flippedX) return rotation + 90
        if (flippedX) return rotation + 180
        return rotation + 270
    }
    property real isSmall: scale < 0.5 ? 1 : 0

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

    StateGroup {
        state: pieceModel.state

        states: [
            State {
                name: "60"

                PropertyChanges { target: root; rotation: 60 }
                // flipY is 0 in all states, but we need to make it part of the
                // property changes because PieceSwitchedFlipAnimation changes
                // it temporarily and otherwise it is not guaranteed to be set
                // to 0 again if a state change is triggered while the last
                // animation is still running, for example by selecting Find
                // Move in quick succession.
                PropertyChanges { target: flipY; angle: 0 }
            },
            State {
                name: "90"

                PropertyChanges { target: root; rotation: 90 }
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
                name: "270"

                PropertyChanges { target: root; rotation: 270 }
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
                name: "90flip"

                PropertyChanges { target: root; rotation: 90 }
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
                name: "270flip"

                PropertyChanges { target: root; rotation: 270 }
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
                from: "90,270flip"; to: from
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
                from: "270,90flip"; to: from
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

            ParentChange {
                target: root
                parent: parentUnplayed
                x: parentUnplayed.width / 2
                y: parentUnplayed.height / 2
                scale: scaleUnplayed
            }
        }
    ]
    transitions:
        Transition {
            from: "unplayed,picked,played"; to: from
            enabled: enableAnimations

            SequentialAnimation {
                ParentAnimation {
                    via: gameView

                    NumberAnimation {
                        properties: "x,y,scale"
                        duration: animationDurationMove
                        easing.type: Easing.InOutSine
                    }
                }
                // Workaround for QTBUG-99695 (ParentAnimation with via leaves
                // item at wrong position if target moves; Qt 6.2)
                ScriptAction {
                    script: {
                        switch (state) {
                        case "picked":
                            x = pieceManipulator.width / 2
                            y = pieceManipulator.height / 2
                            break
                        case "played":
                            x = board.mapFromGameX(pieceModel.gameCoord.x) - board.grabImageTarget.x
                            y = board.mapFromGameX(pieceModel.gameCoord.y) - board.grabImageTarget.y
                            break
                        case "unplayed":
                            x = parentUnplayed.width / 2
                            y = parentUnplayed.height / 2
                            break
                        }
                    }
                }
            }
        }
}
