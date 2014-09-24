//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceListFlickable.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Row {
    id: root

    property real pieceAreaSize
    property int color
    property var pieces
    property int nuPiecesLeft
    property int rows: 1
    property bool allPiecesFitInVisible

    signal piecePicked(var piece)

    Image {
        visible: ! allPiecesFitInVisible
        width: 0.03 * root.width; height: root.height
        source: theme.getImage("flick-left")
        sourceSize { width: width; height: height }
        fillMode: Image.PreserveAspectFit
        opacity: flickable.atXBeginning ? 0 : 1
    }

    Flickable {
        id: flickable

        property int _visibleColumns: root.pieceAreaSize == 0 ?
                                          0 :
                                          Math.floor(width / root.pieceAreaSize)
        property bool _allPiecesLeftFitInVisible:
            nuPiecesLeft <= rows * _visibleColumns

        width: (allPiecesFitInVisible ? 1 : 0.94) * root.width
        height: parent.height
        contentWidth: pieceList.columns * root.pieceAreaSize
        clip: true

        PieceList {
            id: pieceList

            rows: root.rows
            columns: flickable._allPiecesLeftFitInVisible || rows == 0 ?
                         flickable._visibleColumns :
                         Math.ceil(nuPiecesLeft / rows)
            pieceAreaSize: root.pieceAreaSize
            pieces: root.pieces
            onPiecePicked: root.piecePicked(piece)
            onColumnsChanged: flickable.contentX = 0
        }

    }

    Image {
        visible: ! allPiecesFitInVisible
        width: 0.03 * root.width; height: root.height
        source: theme.getImage("flick-right")
        sourceSize { width: width; height: height }
        fillMode: Image.PreserveAspectFit
        opacity: flickable.atXEnd ? 0 : 1
    }
}
