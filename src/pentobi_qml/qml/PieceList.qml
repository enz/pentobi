import QtQuick 2.0

Grid {
    property real pieceAreaSize
    property var pieces

    signal piecePicked(var piece)

    flow: Grid.LeftToRight

    Repeater {
        model: pieces

        MouseArea {
            width: pieceAreaSize
            height: pieceAreaSize
            onClicked: piecePicked(modelData)
            Component.onCompleted: modelData.parentPieceArea = this
        }
    }
}
