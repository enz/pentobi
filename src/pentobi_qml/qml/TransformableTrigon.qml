import QtQuick 2.0

// See TransformableClassic.qml for comments
Item {
    id: root

    property alias transformState: stateGroup.state
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

    StateGroup {
        id: stateGroup

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
                from: ",rot60,rot120,rot180,rot240,rot300"; to: from
                enabled: transitionsEnabled
                PieceRotationAnimation { }
            },
            Transition {
                from: "flip,rot60Flip,rot120Flip,rot180Flip,rot240Flip,rot300Flip"; to: from
                enabled: transitionsEnabled
                PieceRotationAnimation { }
            },
            Transition {
                from: ",flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: "rot60,rot60Flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: "rot120,rot120Flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: "rot180,rot180Flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: "rot240,rot240Flip"; to: from
                enabled: transitionsEnabled
                PieceFlipAnimation { target: flipX }
            },
            Transition {
                from: "rot300,rot300Flip"; to: from
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
                from: "rot60,rot240Flip"; to: from
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
                from: "rot120,rot300Flip"; to: from
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
                from: "rot240,rot60Flip"; to: from
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
                from: "rot300,rot120Flip"; to: from
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
}
