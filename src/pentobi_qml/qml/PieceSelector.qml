//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceSelector.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.1

Item {
    id: root

    property real pieceAreaSize
    property int toPlay
    property var pieces0
    property var pieces1
    property var pieces2
    property var pieces3
    property int nuPiecesLeft0
    property int nuPiecesLeft1
    property int nuPiecesLeft2
    property int nuPiecesLeft3
    property int nuColors
    property int rows: 1
    property bool allPiecesFitInVisible

    signal piecePicked(var piece)

    function showColor(color) { flickable.showColor(color, 80) }

    onToPlayChanged: flickable.showToPlay(80)

    Flickable {
        id: flickable

        anchors.fill: root
        contentHeight: nuColors * height
        flickableDirection: Flickable.VerticalFlick
        clip: true
        onMovementEnded:
            showColor(Math.min(Math.round(contentY / height), nuColors - 1), 200)
        onWidthChanged: showToPlay(0)
        onHeightChanged: showToPlay(0)
        Component.onCompleted: showToPlay(0)

        function showColor(color, duration) {
            snapAnimation.duration = duration
            snapAnimation.to = height * color
            snapAnimation.restart()
        }
        function showToPlay(duration) { showColor(toPlay, duration) }

        NumberAnimation {
            id: snapAnimation

            target: flickable
            property: "contentY"
        }
        Column {
            PieceListFlickable {
                width: root.width
                height: root.height
                rows: root.rows
                allPiecesFitInVisible: root.allPiecesFitInVisible
                pieces: root.pieces0
                nuPiecesLeft: root.nuPiecesLeft0
                pieceAreaSize: root.pieceAreaSize
                onPiecePicked: root.piecePicked(piece)
            }
            PieceListFlickable {
                width: root.width
                height: root.height
                rows: root.rows
                allPiecesFitInVisible: root.allPiecesFitInVisible
                pieces: root.pieces1
                nuPiecesLeft: root.nuPiecesLeft1
                pieceAreaSize: root.pieceAreaSize
                onPiecePicked: root.piecePicked(piece)
            }
            PieceListFlickable {
                visible: nuColors >= 3
                width: root.width
                height: root.height
                rows: root.rows
                allPiecesFitInVisible: root.allPiecesFitInVisible
                pieces: root.pieces2
                nuPiecesLeft: root.nuPiecesLeft2
                pieceAreaSize: root.pieceAreaSize
                onPiecePicked: root.piecePicked(piece)
            }
            PieceListFlickable {
                visible: nuColors >= 4
                width: root.width
                height: root.height
                rows: root.rows
                allPiecesFitInVisible: root.allPiecesFitInVisible
                pieces: root.pieces3
                nuPiecesLeft: root.nuPiecesLeft3
                pieceAreaSize: root.pieceAreaSize
                onPiecePicked: root.piecePicked(piece)
            }
        }
    }
}
