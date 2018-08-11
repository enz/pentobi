//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceTrigon.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

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
    property string imageName: theme.getImage("triangle-" + colorName)
    property string imageNameDownward:
        theme.getImage("triangle-down-" + colorName)
    property bool flippedX: Math.abs(flipX.angle % 360 - 180) < 90
    property bool flippedY: Math.abs(flipY.angle % 360 - 180) < 90
    property real pieceAngle: {
        if (! flippedY && ! flippedX) return rotation
        if (! flippedY && flippedX) return rotation + 120
        if (flippedX) return rotation + 180
        return rotation + 300
    }
    property real imageOpacity0: imageOpacity(pieceAngle, 0)
    property real imageOpacity60: imageOpacity(pieceAngle, 60)
    property real imageOpacity120: imageOpacity(pieceAngle, 120)
    property real imageOpacity180: imageOpacity(pieceAngle, 180)
    property real imageOpacity240: imageOpacity(pieceAngle, 240)
    property real imageOpacity300: imageOpacity(pieceAngle, 300)

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
        var angle = ((pieceAngle - imgAngle) % 360 + 360) % 360
        return angle >= 60 && angle <= 300 ? 0 : 2 * Math.cos(angle * Math.PI / 180) - 1
    }

    Repeater {
        model: pieceModel.elements

        Triangle {
            isDownward: _isDownward(modelData)
            width: 2 * gridWidth
            height: gridHeight
            x: (modelData.x - pieceModel.center.x - 0.5) * gridWidth
            y: (modelData.y - pieceModel.center.y) * gridHeight
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
        y: (pieceModel.labelPos.y - pieceModel.center.y
            + (_isDownward(pieceModel.labelPos) ? 1 : 2) / 3)
           * gridHeight - height / 2
        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
    }
    Text {
        id: labelText

        opacity: text === "" ? 0 : 1
        color: colorName == "blue" || colorName == "red" ?
                   "white" : "#333333"
        font.pixelSize: 0.5 * gridHeight
        width: 0
        height: 0
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        x: (pieceModel.labelPos.x - pieceModel.center.x + 0.5) * gridWidth
        y: (pieceModel.labelPos.y - pieceModel.center.y
            + (_isDownward(pieceModel.labelPos) ? 1 : 2) / 3) * gridHeight
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
                name: "rot60"
                PropertyChanges { target: root; rotation: 60 }
            },
            State {
                name: "rot120"
                PropertyChanges { target: root; rotation: 120 }
            },
            State {
                name: "rot180"
                PropertyChanges { target: root; rotation: 180 }
            },
            State {
                name: "rot240"
                PropertyChanges { target: root; rotation: 240 }
            },
            State {
                name: "rot300"
                PropertyChanges { target: root; rotation: 300 }
            },
            State {
                name: "flip"
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot60Flip"
                PropertyChanges { target: root; rotation: 60 }
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot120Flip"
                PropertyChanges { target: root; rotation: 120 }
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot180Flip"
                PropertyChanges { target: root; rotation: 180 }
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot240Flip"
                PropertyChanges { target: root; rotation: 240 }
                PropertyChanges { target: flipX; angle: 180 }
            },
            State {
                name: "rot300Flip"
                PropertyChanges { target: root; rotation: 300 }
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
                from: "rot60,rot240Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot120,rot300Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot180,flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot240,rot60Flip"; to: from
                enabled: enableAnimations

                PieceSwitchedFlipAnimation { }
            },
            Transition {
                from: "rot300,rot120Flip"; to: from
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
               scale: 0.13 * parentUnplayed.width / gridWidth
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
                NumberAnimation {
                    properties: "x,y,scale"
                    duration: animationDurationMove
                    easing.type: Easing.InOutSine
                }
            }
    }
}
