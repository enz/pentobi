//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceSelector.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property real pieceAreaSize
    property int toPlay
    property var pieces0
    property var pieces1
    property var pieces2
    property var pieces3
    property int rows: 1

    signal piecePicked(var piece)

    PieceListFlickable {
        anchors.fill: root
        rows: root.rows
        pieces: root.pieces0
        isToPlay: root.toPlay == 0
        pieceAreaSize: root.pieceAreaSize
        onPiecePicked: root.piecePicked(piece)
    }
    PieceListFlickable {
        anchors.fill: root
        rows: root.rows
        pieces: root.pieces1
        isToPlay: root.toPlay == 1
        pieceAreaSize: root.pieceAreaSize
        onPiecePicked: root.piecePicked(piece)
    }
    PieceListFlickable {
        anchors.fill: root
        rows: root.rows
        pieces: root.pieces2
        isToPlay: root.toPlay == 2
        pieceAreaSize: root.pieceAreaSize
        onPiecePicked: root.piecePicked(piece)
    }
    PieceListFlickable {
        anchors.fill: root
        rows: root.rows
        pieces: root.pieces3
        isToPlay: root.toPlay == 3
        pieceAreaSize: root.pieceAreaSize
        onPiecePicked: root.piecePicked(piece)
    }
}
