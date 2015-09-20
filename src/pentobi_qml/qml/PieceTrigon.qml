import QtQuick 2.0

// See PieceClassic.qml for comments
Item
{
    id: root

    property var pieceModel
    property string colorName
    property bool isPicked
    property Item parentPieceSelectorArea
    property Item parentPieceManipulator
    property Item parentBoard
    property Item parentAnimationVia
    property real gridElementWidth
    property real gridElementHeight
    property bool isMarked

    property string imageName: theme.getImage("triangle-" + colorName)
    property string imageNameDownward:
        theme.getImage("triangle-down-" + colorName)
    property real pieceAngle: {
        var flX = Math.abs(flipX.angle % 360 - 180) < 90
        var flY = Math.abs(flipY.angle % 360 - 180) < 90
        var angle = rotation
        if (flX && flY) angle += 180
        else if (flX) angle += 120
        else if (flY) angle += 300
        return angle
    }
    property real _elementWidth: 2 * gridElementWidth

    state: {
        if (isPicked) return "picked"
        else if (pieceModel.isPlayed) return "played"
        else if (parentPieceSelectorArea != null) return "unplayed"
        else return ""
    }
    z: 1
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

    function _isDownward(pos) { return (pos.x % 2 == 0) != (pos.y % 2 == 0) }

    Repeater {
        model: pieceModel.elements

        Triangle {
            isDownward: _isDownward(modelData)
            width: root._elementWidth
            height: root.gridElementHeight
            x: (modelData.x - pieceModel.center.x - 0.5) * gridElementWidth
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
           * gridElementWidth + - width / 2
        y: (pieceModel.labelPos.y - pieceModel.center.y
            + (root._isDownward(pieceModel.labelPos) ? 1 : 2) / 3)
           * gridElementHeight - height / 2
        Behavior on opacity { NumberAnimation { duration: 80 } }
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

    states: [
        State {
            name: "unplayed"
            PropertyChanges {
                target: root
                gridElementWidth: 0.13 * parentPieceSelectorArea.width
                gridElementHeight: Math.sqrt(3) * gridElementWidth
            }
            PropertyChanges { target: parentPieceSelectorArea; visible: true }
            ParentChange {
                target: root
                parent: parentPieceSelectorArea
                x: parentPieceSelectorArea.width / 2
                y: parentPieceSelectorArea.height / 2
            }
        },
        State {
            name: "picked"
            PropertyChanges {
                target: root
                gridElementWidth: parentBoard.gridElementWidth
                gridElementHeight: parentBoard.gridElementHeight
            }
            PropertyChanges { target: parentPieceSelectorArea; visible: true }
            ParentChange {
                target: root
                parent: parentPieceManipulator
                x: parentPieceManipulator.width / 2
                y: parentPieceManipulator.height / 2
            }
        },
        State {
            name: "played"
            PropertyChanges {
                target: root
                gridElementWidth: parentBoard.gridElementWidth
                gridElementHeight: parentBoard.gridElementHeight
            }
            PropertyChanges { target: parentPieceSelectorArea; visible: false }
            ParentChange {
                target: root
                parent: parentBoard
                x: parentBoard.mapFromGameX(pieceModel.gameCoord.x)
                y: parentBoard.mapFromGameY(pieceModel.gameCoord.y)
            }
        }
    ]

    transitions:
        Transition {
            from: "unplayed,picked,played"; to: from
            enabled: transitionsEnabled

            SequentialAnimation {
                PropertyAction {
                    target: parentPieceSelectorArea
                    property: "visible"; value: true
                }
                ParentAnimation {
                    via: parentAnimationVia
                    NumberAnimation {
                        properties: "x,y,gridElementWidth,gridElementHeight"
                        duration: 300
                        easing.type: Easing.OutQuad
                    }
                }
                PropertyAction {
                    target: parentPieceSelectorArea; property: "visible"
                }
            }
        }
}
