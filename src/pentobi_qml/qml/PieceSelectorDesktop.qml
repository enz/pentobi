import QtQuick 2.0

Column {
    id: root

    // Dummy, it exists in PieceSelectorMobile and GameDisplay.js uses it
    property bool transitionsEnabled

    property int toPlay: gameModel.toPlay
    property var pieces0
    property var pieces1
    property var pieces2
    property var pieces3
    property int rows: 2
    property int columns: Math.ceil(pieces0.length / 2)

    signal piecePicked(var piece)

    PieceList {
        id: pieceList0

        width: parent.width
        columns: parent.columns
        rows: parent.rows
        pieces: pieces0
        onPiecePicked: root.piecePicked(piece)
    }
    PieceList {
        id: pieceList1

        width: parent.width
        columns: parent.columns
        rows: parent.rows
        pieces: pieces1
        onPiecePicked: root.piecePicked(piece)
    }
    PieceList {
        id: pieceList2

        width: parent.width
        columns: parent.columns
        rows: parent.rows
        pieces: pieces2
        onPiecePicked: root.piecePicked(piece)
    }
    PieceList {
        id: pieceList3

        width: parent.width
        columns: parent.columns
        rows: parent.rows
        pieces: pieces3
        onPiecePicked: root.piecePicked(piece)
    }
}
