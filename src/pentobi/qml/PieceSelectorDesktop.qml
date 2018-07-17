import QtQuick 2.0

Item {
    id: root

    property string gameVariant
    property int toPlay
    property alias pieces0: pieceList0.pieces
    property alias pieces1: pieceList1.pieces
    property alias pieces2: pieceList2.pieces
    property alias pieces3: pieceList3.pieces
    property int nuColors
    property int columns: pieces0 ? Math.ceil(pieces0.length / 2) : 11

    /** Dummy for compatibility with PieceSelectorMobile */
    property bool transitionsEnabled

    signal piecePicked(var piece)

    Column {
        width: Math.min(parent.width, parent.height / 8.15 * columns)
        height: Math.min(parent.height, parent.width / columns * 8.15)
        spacing: height / 8 * 0.05
        anchors.centerIn: parent

        PieceList {
            id: pieceList0

            width: parent.width
            columns: root.columns
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            id: pieceList1

            width: parent.width
            columns: root.columns
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            id: pieceList2

            width: parent.width
            columns: root.columns
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            id: pieceList3

            width: parent.width
            columns: root.columns
            onPiecePicked: root.piecePicked(piece)
        }
    }
}
