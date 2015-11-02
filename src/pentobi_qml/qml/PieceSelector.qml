import QtQuick 2.0

Flickable {
    id: root

    property int toPlay
    property var pieces0
    property var pieces1
    property var pieces2
    property var pieces3
    property int nuColors
    property int columns
    property int rows

    signal piecePicked(var piece)

    function showColorImmediately(color) {
        snapAnimation.stop()
        showColorAnimation.stop()
        contentY = column.children[color].y
    }
    function showToPlay() {
        if (! transitionsEnabled) {
            showColorImmediately(toPlay)
            return
        }
        snapAnimation.stop()
        showColorAnimation.stop()
        showColorMoveAnimation.to = column.children[toPlay].y
        showColorAnimation.restart()
    }

    onToPlayChanged: showToPlay()
    contentHeight: nuColors * height
    flickableDirection: Flickable.VerticalFlick
    clip: true
    onMovementEnded: {
        var d = 0.5 * width / columns
        for (var i = 0; i < nuColors; ++i) {
            var y = column.children[i].y
            if (Math.abs(contentY - y) < d) {
                showColorAnimation.stop()
                snapAnimation.to = y
                snapAnimation.restart()
                break
            }
        }
    }
    onWidthChanged: showColorImmediately(toPlay)
    onHeightChanged: showColorImmediately(toPlay)

    SmoothedAnimation {
        id: snapAnimation

        target: root
        property: "contentY"
        velocity: 200
    }
    SequentialAnimation {
        id: showColorAnimation

        PauseAnimation { duration: 350 }
        NumberAnimation {
            id: showColorMoveAnimation

            target: root
            property: "contentY"
            duration: 50
        }
    }
    Column {
        id: column

        onHeightChanged: showColorImmediately(toPlay)

        PieceList {
            width: root.width
            columns: root.columns
            pieces: pieces0
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            width: root.width
            columns: root.columns
            pieces: pieces1
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            visible: nuColors >= 3
            width: root.width
            columns: root.columns
            pieces: pieces2
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            visible: nuColors >= 4
            width: root.width
            columns: root.columns
            pieces: pieces3
            onPiecePicked: root.piecePicked(piece)
        }
    }
}
