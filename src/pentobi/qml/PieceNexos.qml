//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceNexos.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// Piece for Nexos. See PieceClassic for comments.
Item
{
    id: root

    property QtObject pieceModel
    property string colorName
    property bool isPicked
    property Item parentUnplayed
    property real gridWidth: board.gridWidth
    property real gridHeight: board.gridHeight
    property bool isMarked
    property string label
    property string imageName: theme.getImage("linesegment-" + colorName)
    property bool flippedX: Math.abs(flipX.angle % 360 - 180) < 90
    property bool flippedY: Math.abs(flipY.angle % 360 - 180) < 90
    property real pieceAngle: {
        if (! flippedY && ! flippedX) return rotation
        if (! flippedY && flippedX) return rotation + 90
        if (flippedX) return rotation + 180
        return rotation + 270
    }
    property real imageOpacity0: imageOpacity(pieceAngle, 0)
    property real imageOpacity90: imageOpacity(pieceAngle, 90)
    property real imageOpacity180: imageOpacity(pieceAngle, 180)
    property real imageOpacity270: imageOpacity(pieceAngle, 270)

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

    function isHorizontal(pos) { return pos.x % 2 != 0 }
    function imageOpacity(pieceAngle, imgAngle) {
        var angle = ((pieceAngle - imgAngle) % 360 + 360) % 360
        return angle >= 90 && angle <= 270 ? 0 : Math.cos(angle * Math.PI / 180)
    }

    Repeater {
        model: pieceModel.elements

        LineSegment {
            isHorizontal: root.isHorizontal(modelData)
            width: 1.5 * gridWidth
            height: 0.5 * gridHeight
            x: (modelData.x - pieceModel.center.x - 0.25) * gridWidth
            y: (modelData.y - pieceModel.center.y + 0.25) * gridHeight
        }
    }
    Repeater {
        model: pieceModel.junctions

        PieceElementImage {
            source: {
                switch (pieceModel.junctionType[index]) {
                case 0:
                    return theme.getImage("junction-all-" + colorName)
                case 1:
                case 2:
                case 3:
                case 4:
                    return theme.getImage("junction-t-" + colorName)
                case 5:
                case 6:
                    return theme.getImage("junction-straight-" + colorName)
                case 7:
                case 8:
                case 9:
                case 10:
                    return theme.getImage("junction-rect-" + colorName)
                }
            }
            rotation: {
                switch (pieceModel.junctionType[index]) {
                case 0:
                case 3:
                case 5:
                case 10:
                    return 0
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
                }
            }
            width: 0.5 * gridWidth
            height: 0.5 * gridHeight
            x: (modelData.x - pieceModel.center.x + 0.25) * gridWidth
            y: (modelData.y - pieceModel.center.y + 0.25) * gridHeight
            sourceSize: imageSourceSize
        }
    }
    Rectangle {
        opacity: isMarked ? 0.5 : 0
        color: gameModel.showVariations && ! gameModel.isMainVar ? "transparent" : border.color
        border.width: 0.2 * width
        border.color: colorName == "blue" || colorName == "red" ? "white" : "#333333"
        width: 0.3 * gridHeight
        height: width
        radius: width / 2
        x: (pieceModel.labelPos.x - pieceModel.center.x + 0.5)
           * gridWidth - width / 2
        y: (pieceModel.labelPos.y - pieceModel.center.y + 0.5)
           * gridHeight - height / 2
        Behavior on opacity { NumberAnimation { duration: 80 } }
    }
    Text {
        opacity: label == "" ? 0 : 1
        text: label
        color: colorName == "blue" || colorName == "red" ?
                   "white" : "#333333"
        font.pixelSize: 0.5 * gridHeight
        width: 0
        height: 0
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        x: (pieceModel.labelPos.x - pieceModel.center.x + 0.5) * gridWidth
        y: (pieceModel.labelPos.y - pieceModel.center.y + 0.5) * gridHeight
        transform: [
            Rotation {
                axis { x: 0; y: 1; z: 0 }
                angle: -flipY.angle
            },
            Rotation {
                axis { x: 1; y: 0; z: 0 }
                angle: -flipX.angle
            },
            Rotation {
                angle: -root.rotation
            }
        ]
        Behavior on opacity { NumberAnimation { duration: 80 } }
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
            when: isPicked

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
                parent: board
                x: board.mapFromGameX(pieceModel.gameCoord.x)
                y: board.mapFromGameY(pieceModel.gameCoord.y)
            }
        },
        State {
            name: "unplayed"
            when: parentUnplayed != null

            PropertyChanges {
                target: root
                // Avoid fractional sizes for square piece elements
                scale: Math.floor(0.12 * parentUnplayed.width) / gridWidth
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

            ParentAnimation {
                via: gameDisplay
                NumberAnimation {
                    properties: "x,y,scale"
                    duration: 300
                    easing.type: Easing.InOutSine
                }
            }
    }
}
