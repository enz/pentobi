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
        flow: Grid.LeftToRight

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
            }
        }
    }
}
