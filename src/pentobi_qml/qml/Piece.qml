//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/Piece.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item
{
    id: root

    property var pieceModel

    property string gameVariant

    property int color: pieceModel.color

    property bool isPicked

    property Item parentPieceSelectorArea

    property Item parentPieceManipulator

    property Item parentBoard

    property Item parentAnimationVia

    property real gridElementWidth
    property real gridElementHeight
    property real imageSourceWidth
    property real imageSourceHeight

    property bool _isTrigon: gameVariant.indexOf("trigon") >= 0

    state: {
        if (isPicked) return "picked"
        else if (pieceModel.isPlayed) return "played"
        else if (parentPieceSelectorArea != null) return "unplayed"
        else return ""
    }

    PieceShadow {
        id: pieceShadow
        gameVariant: root.gameVariant
        elements: pieceModel.elements
        center: pieceModel.center
        state: pieceModel.state
        gridElementWidth: root.gridElementWidth
        gridElementHeight: root.gridElementHeight
        imageSourceWidth: root.imageSourceWidth
        imageSourceHeight: root.imageSourceHeight
        x: 0.1 * gridElementWidth; y: 0.1 * gridElementHeight
        opacity: 0
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }

    PieceShape {
        z: 1
        gameVariant: root.gameVariant
        color: root.color
        elements: pieceModel.elements
        center: pieceModel.center
        state: pieceModel.state
        gridElementWidth: root.gridElementWidth
        gridElementHeight: root.gridElementHeight
        imageSourceWidth: root.imageSourceWidth
        imageSourceHeight: root.imageSourceHeight
    }

    states: [
        State {
            name: "unplayed"
            PropertyChanges {
                target: root
                // Avoid fractional sizes for square piece elements
                gridElementWidth: _isTrigon ? parentPieceSelectorArea.width / 7 : Math.floor(parentPieceSelectorArea.width / 5)
                gridElementHeight: _isTrigon ? Math.sqrt(3) * gridElementWidth : gridElementWidth
                opacity: 0.9
            }
            PropertyChanges { target: pieceShadow; opacity: 0 }
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
                opacity: 1
            }
            PropertyChanges { target: pieceShadow; opacity :0.15 }
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
                opacity: 1
            }
            PropertyChanges { target: pieceShadow; opacity :0 }
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
            SequentialAnimation {
                // Temporarily set z to 3 such that it is above the pieces
                // on the board and above the piece manipulator
                PropertyAction { target: root; property: "z"; value: 3 }
                PropertyAction { target: pieceShadow; property: "opacity"; value: 0.15 }
                ParentAnimation {
                    via: parentAnimationVia
                    NumberAnimation { properties: "x,y,gridElementWidth,gridElementHeight"; duration: 300 }
                }
                PropertyAction { target: root; property: "z"; value: 0 }
                PropertyAction { target: pieceShadow; property: "opacity" }
            }
        }
}
