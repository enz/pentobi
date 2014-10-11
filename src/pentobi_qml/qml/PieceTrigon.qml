import QtQuick 2.0
import "GameDisplay.js" as Logic

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
    property real pieceAngle: {
        var flipX = Math.abs(pieceElements.flipXAngle % 360 - 180) < 90
        var flipY = Math.abs(pieceElements.flipYAngle % 360 - 180) < 90
        var angle = pieceElements.rotation
        if (flipX && flipY) angle += 180
        else if (flipX) angle += 120
        else if (flipY) angle += 300
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

    function _isDownward(x, y) {
        return x % 2 != 0 ? (y % 2 == 0) : (y % 2 != 0)
    }

    TransformableTrigon {
        id: pieceElements

        state: pieceModel.state

        Repeater {
            model: pieceModel.elements

            PieceElementTrigon {
                isDownward: _isDownward(modelData.x, modelData.y)
                width: root._elementWidth
                height: root.gridElementHeight
                x: (modelData.x - pieceModel.center.x - 0.5)
                   * gridElementWidth
                y: (modelData.y - pieceModel.center.y)
                   * gridElementHeight
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
                + (root._isDownward(pieceModel.labelPos.x,
                                    pieceModel.labelPos.y) ?
                       1 / 3 : 2 / 3))
               * gridElementHeight - height / 2
            Behavior on opacity { NumberAnimation { duration: 80 } }
        }
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
