import QtQuick 2.0

Item
{
    id: root

    property var pieceModel
    property string colorName
    property bool isPicked
    property Item parentPieceArea
    property real gridElementWidth
    property real gridElementHeight
    property bool isMarked

    property string imageName: theme.getImage("square-" + colorName)
    property real pieceAngle: {
        var flX = Math.abs(flipX.angle % 360 - 180) < 90
        var flY = Math.abs(flipY.angle % 360 - 180) < 90
        var angle = rotation
        if (flX && flY) angle += 180
        else if (flX) angle += 90
        else if (flY) angle += 270
        return angle
    }

    // Make sure piece is above board during piece transition when its parent
    // is GameDisplay
    z: 1
    transform: [
        Rotation {
            id: flipX

            axis { x: 1; y: 0; z: 0 }
            origin { x: width / 2; y: height / 2 }
        },
        Rotation {
            id: flipY

            axis { x: 0; y: 1; z: 0 }
            origin { x: width / 2; y: height / 2 }
        }
    ]

    Repeater {
        model: pieceModel.elements

        Square {
            width: gridElementWidth
            height: gridElementHeight
            x: (modelData.x - pieceModel.center.x) * gridElementWidth
            y: (modelData.y - pieceModel.center.y) * gridElementHeight
        }
    }
    Rectangle {
        opacity: isMarked ? 0.5 : 0
        color: colorName == "blue" || colorName == "red" ?
                   "white" : "#333333"
        width: 0.3 * gridElementHeight
        height: width
        radius: width / 2
        x: (pieceModel.labelPos.x - pieceModel.center.x + 0.5)
           * gridElementWidth - width / 2
        y: (pieceModel.labelPos.y - pieceModel.center.y + 0.5)
           * gridElementHeight - height / 2
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

        // Unique states are defined by rotating and flipping around the x axis
        // but for some transitions, the shortest visual animation is flipping
        // around the y axis.
        transitions: [
            Transition {
                from: ",rot90,rot180,rot270"; to: from
                enabled: transitionsEnabled
                PieceRotationAnimation { }
            },
            Transition {
                from: "flip,rot90Flip,rot180Flip,rot270Flip"; to: from
                enabled: transitionsEnabled
                PieceRotationAnimation { }
            },
            Transition {
                from: ",flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: "rot90,rot90Flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: "rot180,rot180Flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: "rot270,rot270Flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: ",rot180Flip"; to: from
                enabled: transitionsEnabled
                SequentialAnimation {
                    PropertyAction { property: "rotation"; value: rotation }
                    PropertyAction {
                        target: flipX; property: "angle"; value: flipX.angle
                    }
                    PieceFlipAnimation { target: flipY; to: 180 }
                    PropertyAction { target: flipY; property: "angle"; value: 0 }
                }
            },
            Transition {
                from: "rot90,rot270Flip"; to: from
                enabled: transitionsEnabled
                SequentialAnimation {
                    PropertyAction { property: "rotation"; value: rotation }
                    PropertyAction {
                        target: flipX; property: "angle"; value: flipX.angle
                    }
                    PieceFlipAnimation { target: flipY; to: 180 }
                    PropertyAction { target: flipY; property: "angle"; value: 0 }
                }
            },
            Transition {
                from: "rot180,flip"; to: from
                enabled: transitionsEnabled
                SequentialAnimation {
                    PropertyAction { property: "rotation"; value: rotation }
                    PropertyAction {
                        target: flipX; property: "angle"; value: flipX.angle
                    }
                    PieceFlipAnimation { target: flipY; to: 180 }
                    PropertyAction { target: flipY; property: "angle"; value: 0 }
                }
            },
            Transition {
                from: "rot270,rot90Flip"; to: from
                enabled: transitionsEnabled
                SequentialAnimation {
                    PropertyAction { property: "rotation"; value: rotation }
                    PropertyAction {
                        target: flipX; property: "angle"; value: flipX.angle
                    }
                    PieceFlipAnimation { target: flipY; to: 180 }
                    PropertyAction { target: flipY; property: "angle"; value: 0 }
                }
            }
        ]
    }

    states: [
        State {
            name: "picked"
            when: isPicked

            PropertyChanges {
                target: root
                gridElementWidth: board.gridElementWidth
                gridElementHeight: board.gridElementHeight
            }
            PropertyChanges { target: parentPieceArea; visible: true }
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

            PropertyChanges {
                target: root
                gridElementWidth: board.gridElementWidth
                gridElementHeight: board.gridElementHeight
            }
            PropertyChanges { target: parentPieceArea; visible: false }
            ParentChange {
                target: root
                parent: board
                x: board.mapFromGameX(pieceModel.gameCoord.x)
                y: board.mapFromGameY(pieceModel.gameCoord.y)
            }
        },
        State {
            name: "unplayed"
            when: parentPieceArea != null

            PropertyChanges {
                target: root
                // Avoid fractional sizes for square piece elements
                gridElementWidth: Math.floor(0.2 * parentPieceArea.width)
                gridElementHeight: gridElementWidth
            }
            PropertyChanges { target: parentPieceArea; visible: true }
            ParentChange {
                target: root
                parent: parentPieceArea
                x: parentPieceArea.width / 2
                y: parentPieceArea.height / 2
            }
        }
    ]
    transitions:
        Transition {
            from: "unplayed,picked,played"; to: from
            enabled: transitionsEnabled

            SequentialAnimation {
                PropertyAction {
                    target: parentPieceArea
                    property: "visible"; value: true
                }
                ParentAnimation {
                    via: gameDisplay
                    NumberAnimation {
                        properties: "x,y,gridElementWidth,gridElementHeight"
                        duration: 300
                        easing.type: Easing.OutQuad
                    }
                }
                PropertyAction {
                    target: parentPieceArea; property: "visible"
                }
            }
        }
}
