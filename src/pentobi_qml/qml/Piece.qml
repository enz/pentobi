//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/Piece.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import "GameDisplay.js" as Logic

Item
{
    id: root

    property var pieceModel
    property bool isTrigon
    property string colorName
    property bool isPicked
    property Item parentPieceSelectorArea
    property Item parentPieceManipulator
    property Item parentBoard
    property Item parentAnimationVia
    property real gridElementWidth
    property real gridElementHeight
    property bool isMarked

    property string _imageName:
        theme.getImage((isTrigon ? "triangle-" : "square-") + colorName)
    property string _imageNameDownward:
        theme.getImage("triangle-down-" + colorName)
    property bool _fastRendering

    state: {
        if (isPicked) return "picked"
        else if (pieceModel.isPlayed) return "played"
        else if (parentPieceSelectorArea != null) return "unplayed"
        else return ""
    }

    Transformable {
        id: pieceShape

        state: pieceModel.state

        property real _angle: {
            var flipX = Math.abs(pieceShape.flipXAngle % 360 - 180) < 90
            var flipY = Math.abs(pieceShape.flipYAngle % 360 - 180) < 90
            var angle = pieceShape.rotation
            if (isTrigon) {
                if (flipX && flipY) angle += 180
                else if (flipX) angle += 120
                else if (flipY) angle += 300
            }
            else {
                if (flipX && flipY) angle += 180
                else if (flipX) angle += 90
                else if (flipY) angle += 270
            }
            return angle
        }
        property real _elementWidth:
            isTrigon ? 2 * gridElementWidth : gridElementWidth

        Item {
            id: pieceElements

            width: 10 * gridElementWidth
            height: 10 * gridElementHeight
            x: -width / 2
            y: -height / 2

            Repeater {
                model: pieceModel.elements

                PieceElement {
                    imageName: root._imageName
                    imageNameDownward: root._imageNameDownward
                    fastRendering: root._fastRendering
                    isTrigon: root.isTrigon
                    isDownward: isTrigon && (modelData.x % 2 != 0 ?
                                                 (modelData.y % 2 == 0) :
                                                 (modelData.y % 2 != 0))
                    width: pieceShape._elementWidth
                    height: root.gridElementHeight
                    x: (isTrigon ?
                            modelData.x - pieceModel.center.x - 0.5 :
                            modelData.x - pieceModel.center.x)
                       * gridElementWidth + pieceElements.width / 2
                    y: (modelData.y - pieceModel.center.y)
                       * gridElementHeight + pieceElements.height / 2
                    angle: pieceShape._angle
                }
            }
            Rectangle {
                opacity: isMarked ? 0.5 : 0
                color: colorName == "blue" || colorName == "red" ?
                           "white" : "#333333"
                width: 0.3 * gridElementHeight
                height: width
                radius: width / 2
                x: (-pieceModel.center.x + 0.5) * gridElementWidth +
                   pieceElements.width / 2 - width / 2
                y: (isTrigon ?
                        (-pieceModel.center.y + 2 / 3) * gridElementHeight :
                        (-pieceModel.center.y + 0.5) * gridElementHeight) +
                   pieceElements.height / 2 - height / 2
                Behavior on opacity { NumberAnimation { duration: 80 } }
            }
        }
    }

    states: [
        State {
            name: "unplayed"
            PropertyChanges {
                target: root
                // Avoid fractional sizes for square piece elements
                gridElementWidth:
                    isTrigon ?
                        0.13 * parentPieceSelectorArea.width :
                        Math.floor(0.2 * parentPieceSelectorArea.width)
                gridElementHeight: isTrigon ?
                                       Math.sqrt(3) * gridElementWidth :
                                       gridElementWidth
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
                    target: root; property: "_fastRendering"; value: true }
                // Temporarily set z to 3 such that it is above the pieces
                // on the board and above the piece manipulator
                PropertyAction { target: root; property: "z"; value: 3 }
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
                PropertyAction { target: root; property: "z"; value: 0 }
                PropertyAction {
                    target: root; property: "_fastRendering"; value: false }
            }
        }
}
