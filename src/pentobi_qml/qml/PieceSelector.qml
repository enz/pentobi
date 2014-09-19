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

    /** Enforce that the pieces of a color are shown even if the color is
        not to play.
        Setting to -1 means that there is no such enforcement active. */
    property int forceColor: -1

    signal piecePicked(var piece)

    PieceListFlickable {
        anchors.fill: root
        rows: root.rows
        pieces: root.pieces0
        isShown: (forceColor < 0 && root.toPlay == 0) || forceColor == 0
        delayOpacityChange: forceColor >= 0
        pieceAreaSize: root.pieceAreaSize
        onPiecePicked: root.piecePicked(piece)
    }
    PieceListFlickable {
        anchors.fill: root
        rows: root.rows
        pieces: root.pieces1
        isShown: (forceColor < 0 && root.toPlay == 1) || forceColor == 1
        delayOpacityChange: forceColor >= 0
        pieceAreaSize: root.pieceAreaSize
        onPiecePicked: root.piecePicked(piece)
    }
    PieceListFlickable {
        anchors.fill: root
        rows: root.rows
        pieces: root.pieces2
        isShown: (forceColor < 0 && root.toPlay == 2) || forceColor == 2
        delayOpacityChange: forceColor >= 0
        pieceAreaSize: root.pieceAreaSize
        onPiecePicked: root.piecePicked(piece)
    }
    PieceListFlickable {
        anchors.fill: root
        rows: root.rows
        pieces: root.pieces3
        isShown: (forceColor < 0 && root.toPlay == 3) || forceColor == 3
        delayOpacityChange: forceColor >= 0
        pieceAreaSize: root.pieceAreaSize
        onPiecePicked: root.piecePicked(piece)
    }
}
