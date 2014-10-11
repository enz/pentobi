//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceClassic.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import "GameDisplay.js" as Logic

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

    property string imageName: theme.getImage("square-" + colorName)
    property real pieceAngle: {
        var flipX = Math.abs(pieceElements.flipXAngle % 360 - 180) < 90
        var flipY = Math.abs(pieceElements.flipYAngle % 360 - 180) < 90
        var angle = pieceElements.rotation
        if (flipX && flipY) angle += 180
        else if (flipX) angle += 90
        else if (flipY) angle += 270
        return angle
    }

    state: {
        if (isPicked) return "picked"
        else if (pieceModel.isPlayed) return "played"
        else if (parentPieceSelectorArea != null) return "unplayed"
        else return ""
    }
    // Make sure piece is above board during piece transition when its parent
    // is GameDisplay
    z: 1

    Transformable {
        id: pieceElements

        state: pieceModel.state

        Repeater {
            model: pieceModel.elements

            PieceElementClassic {
                width: root.gridElementWidth
                height: root.gridElementHeight
                x: (modelData.x - pieceModel.center.x)
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
            y: (pieceModel.labelPos.y - pieceModel.center.y + 0.5)
               * gridElementHeight - height / 2
            Behavior on opacity { NumberAnimation { duration: 80 } }
        }
    }

    states: [
        State {
            name: "unplayed"
            PropertyChanges {
                target: root
                // Avoid fractional sizes for square piece elements
                gridElementWidth:
                    Math.floor(0.2 * parentPieceSelectorArea.width)
                gridElementHeight: gridElementWidth
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
