//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceCallisto.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// See PieceClassic.qml for comments
Item
{
    id: root

    property QtObject pieceModel
    property string colorName
    property alias isPicked: statePicked.when
    property Item parentUnplayed
    property real gridWidth: board.gridWidth
    property real gridHeight: board.gridHeight
    property bool isMarked
    property alias label: labelText.text
    property string imageName: pieceModel.elements.length === 1 ?
                                   theme.getImage("frame-" + colorName) :
                                   theme.getImage("square-" + colorName)
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

    function imageOpacity(pieceAngle, imgAngle) {
        var angle = ((pieceAngle - imgAngle) % 360 + 360) % 360
        return angle >= 90 && angle <= 270 ? 0 : Math.cos(angle * Math.PI / 180)
    }

    Repeater {
        model: pieceModel.elements

        Item {
            Square {
                width: 0.9 * gridWidth
                height: 0.9 * gridHeight
                x: (modelData.x - pieceModel.center.x) * gridWidth
                   + (gridWidth - width) / 2
                y: (modelData.y - pieceModel.center.y) * gridHeight
                   + (gridHeight - height) / 2
            }
            // Right junction
            PieceElementImage {
                visible: pieceModel.junctionType[index] === 0
                         || pieceModel.junctionType[index] === 1
                source: theme.getImage("junction-all-" + colorName)
                width: 0.1 * gridWidth
                height: 0.85 * gridHeight
                x: (modelData.x - pieceModel.center.x + 1) * gridWidth
                   - width / 2
                y: (modelData.y - pieceModel.center.y) * gridHeight
                   + (gridHeight - height) / 2
                sourceSize { width: width; height: height }
            }
            // Down junction
            PieceElementImage {
                visible: pieceModel.junctionType[index] === 0
                         || pieceModel.junctionType[index] === 2
                source: theme.getImage("junction-all-" + colorName)
                width: 0.85 * gridWidth
                height: 0.1 * gridHeight
                x: (modelData.x - pieceModel.center.x) * gridWidth
                   + (gridWidth - width) / 2
                y: (modelData.y - pieceModel.center.y + 1) * gridHeight
                   - height / 2
                sourceSize { width: width; height: height }
            }
        }
    }
    Rectangle {
        opacity: isMarked ? 0.5 : 0
        color: gameModel.showVariations && ! gameModel.isMainVar ? "transparent" : border.color
        border.width: 0.2 * width
        border.color: colorName === "blue" || colorName === "red" || root.pieceModel.elements.length === 1 ?
                          "white" : "#333333"
        width: 0.3 * gridHeight
        height: width
        radius: width / 2
        x: (pieceModel.labelPos.x - pieceModel.center.x + 0.5)
           * gridWidth - width / 2
        y: (pieceModel.labelPos.y - pieceModel.center.y + 0.5)
           * gridHeight - height / 2
        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
    }
    Text {
        id: labelText

        opacity: text === "" ? 0 : 1
        color: colorName === "blue" || colorName === "red" || root.pieceModel.elements.length === 1 ?
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
        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
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
            id: statePicked

            name: "picked"

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
                // Avoid fractional sizes for square piece elements
                scale: Math.floor(0.25 * parentUnplayed.width) / gridWidth
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
