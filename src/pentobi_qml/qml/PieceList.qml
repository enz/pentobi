//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceList.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property real pieceAreaSize
    property var pieces
    property int columns
    property int rows

    signal piecePicked(var piece)

    Grid {
        columns: root.columns
        rows: root.rows
        flow: Grid.TopToBottom

        Repeater {
            model: pieces
            Item {
                id: pieceArea

                property var piece: modelData
                property var pieceModel: modelData.pieceModel

                visible: ! pieceModel.isPlayed
                width: pieceAreaSize
                height: pieceAreaSize

                Component.onCompleted: piece.parentPieceSelectorArea = pieceArea

                MouseArea {
                    anchors.fill: pieceArea
                    onClicked: piecePicked(piece)
                }

                Behavior on visible {
                    SequentialAnimation {
                        // Delay a bit such that the empty area disappers only
                        // after the movement animation of the piece view
                        PropertyAction { target: pieceArea; property: "visible"; value: 1 }
                        PauseAnimation { duration: 300 }
                        PropertyAction { target: pieceArea; property: "visible" }
                    }
                }
            }
        }
    }
}
