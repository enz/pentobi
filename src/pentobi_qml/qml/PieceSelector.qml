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
        contentY = height * color
    }
    function showToPlay() {
        if (! transitionsEnabled) {
            showColorImmediately(toPlay)
            return
        }
        snapAnimation.stop()
        showColorAnimation.stop()
        showColorMoveAnimation.to = toPlay * height
        showColorAnimation.restart()
    }

    onToPlayChanged: showToPlay()
    contentHeight: nuColors * height
    flickableDirection: Flickable.VerticalFlick
    clip: true
    // Snap to beginning of pieces if close to it
    onMovementEnded: {
        var normalizedY = contentY / height
        var remainderY = normalizedY - Math.floor(normalizedY)
        if (remainderY > 0.1 && remainderY < 0.9)
            return
        var color = Math.min(Math.round(normalizedY), nuColors - 1)
        showColorAnimation.stop()
        snapAnimation.to = height * color
        snapAnimation.restart()
    }
    onWidthChanged: showColorImmediately(toPlay)
    onHeightChanged: showColorImmediately(toPlay)
    Component.onCompleted: showColorImmediately(toPlay)

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
        PieceList {
            width: root.width; height: root.height
            columns: root.columns; rows: root.rows
            pieces: pieces0
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            width: root.width; height: root.height
            columns: root.columns; rows: root.rows
            pieces: pieces1
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            visible: nuColors >= 3
            width: root.width; height: root.height
            columns: root.columns; rows: root.rows
            pieces: pieces2
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            visible: nuColors >= 4
            width: root.width; height: root.height
            columns: root.columns; rows: root.rows
            pieces: pieces3
            onPiecePicked: root.piecePicked(piece)
        }
    }
}
