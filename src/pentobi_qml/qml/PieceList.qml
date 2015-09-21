import QtQuick 2.0

Grid {
    property real pieceAreaSize
    property var pieces

    signal piecePicked(var piece)

    flow: Grid.LeftToRight

    Repeater {
        model: pieces

        MouseArea {
            id: pieceArea

            property var piece: modelData

            width: pieceAreaSize
            height: pieceAreaSize
            Component.onCompleted: piece.parentPieceArea = pieceArea
            onClicked: piecePicked(piece)
        }
    }
}
