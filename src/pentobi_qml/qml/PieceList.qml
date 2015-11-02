import QtQuick 2.0

Grid {
    id: root

    property var pieces

    signal piecePicked(var piece)

    opacity : 0.9

    Repeater {
        model: pieces

        MouseArea {
            width: root.width / columns
            height: width
            onClicked: piecePicked(modelData)
            Component.onCompleted: modelData.parentPieceArea = this
        }
    }
}
