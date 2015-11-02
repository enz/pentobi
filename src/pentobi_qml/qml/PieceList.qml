import QtQuick 2.0

Grid {
    id: root

    property var pieces

    signal piecePicked(var piece)

    Repeater {
        model: pieces

        MouseArea {
            width: root.width / columns
            height: root.height / rows
            onClicked: piecePicked(modelData)
            Component.onCompleted: modelData.parentPieceArea = this
        }
    }
}
