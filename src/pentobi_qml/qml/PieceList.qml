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
            onClicked: piecePicked(modelData)
            Component.onCompleted: modelData.parentUnplayed = mouseArea
        }
    }
}
