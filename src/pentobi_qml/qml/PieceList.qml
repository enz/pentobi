import QtQuick 2.0

Grid {
    id: root

    property real pieceAreaSize
    property var pieces

    signal piecePicked(var piece)

    flow: Grid.LeftToRight

    Repeater {
        model: pieces
        Item {
            id: pieceArea

            property var piece: modelData

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
