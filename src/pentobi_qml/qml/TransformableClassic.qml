import QtQuick 2.0

Item {
    id: root

    property alias flipXAngle: flipX.angle
    property alias flipYAngle: flipY.angle

    transform: [
        Rotation {
            id: flipX
            axis { x: 1; y: 0; z: 0 }
            origin { x: root.width / 2; y: root.height / 2 }
        },
        Rotation {
            id: flipY
            axis { x: 0; y: 1; z: 0 }
            origin { x: root.width / 2; y: root.height / 2 }
        }
    ]

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
