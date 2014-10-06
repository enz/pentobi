//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceSelector.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

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

    function showColorImmediately(color) {
        snapAnimation.stop()
        showColorAnimation.stop()
        flickable.contentY = height * color
    }

    onToPlayChanged: {
        if (! transitionsEnabled) {
            showColorImmediately(toPlay)
            return
        }
        snapAnimation.stop()
        showColorAnimation.stop()
        showColorMoveAnimation.to = toPlay * height
        showColorAnimation.restart()
    }


    Flickable {
        id: flickable

        anchors.fill: root
        contentHeight: nuColors * height
        flickableDirection: Flickable.VerticalFlick
        clip: true
        onMovementEnded: {
            var color = Math.min(Math.round(contentY / height), nuColors - 1)
            showColorAnimation.stop()
            snapAnimation.to = height * color
            snapAnimation.restart()
        }
        onWidthChanged: showColorImmediately(toPlay)
        onHeightChanged: showColorImmediately(toPlay)
        Component.onCompleted: showColorImmediately(toPlay)

        SmoothedAnimation {
            id: snapAnimation

            target: flickable
            property: "contentY"
            velocity: 200
        }
        SequentialAnimation {
            id: showColorAnimation

            PauseAnimation { duration: 350 }
            NumberAnimation {
                id: showColorMoveAnimation

                target: flickable
                property: "contentY"
                duration: 80
            }
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
