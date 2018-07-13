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

            width: root.width / columns; height: width
            visible: ! modelData.pieceModel.isPlayed
            onClicked: {
                gameDisplay.dropCommentFocus()
                piecePicked(modelData)
            }
            hoverEnabled: isDesktop
            scale: isDesktop && containsMouse
                   && ! playerModel.isGenMoveRunning && ! gameModel.isGameOver
                   && (modelData.pieceModel.color === gameModel.toPlay || setupMode)
                   ? 1.2 : 1.0
            Component.onCompleted: modelData.parentUnplayed = mouseArea

            Behavior on scale { NumberAnimation { duration: 20 } }
        }
    }
}
