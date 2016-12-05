import QtQuick 2.0

Grid {
    id: root

    property var pieces

    signal piecePicked(var piece)

    opacity: theme.pieceListOpacity

    Repeater {
        model: pieces

        MouseArea {
            id: mouseArea

            property var piece: modelData

            width: root.width / columns; height: width
            visible: ! piece.pieceModel.isPlayed
            onClicked: piecePicked(piece)
            Component.onCompleted: piece.parentUnplayed = mouseArea
        }
    }
}
