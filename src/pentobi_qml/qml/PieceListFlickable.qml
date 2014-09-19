//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceListFlickable.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Row {
    id: root

    property real pieceAreaSize
    property int color
    property bool isShown

    /** Delay changes to opacity a bit such that the pieces of the new color
        are only visible after the movement animation of the piece view. */
    property bool delayOpacityChange

    property var pieces
    property int rows: 1

    signal piecePicked(var piece)

    z: isShown ? 1 : 0
    opacity: isShown ? 1 : 0

    Behavior on opacity {
        SequentialAnimation {
            PauseAnimation { duration: delayOpacityChange ? 300 : 0 }
            PropertyAnimation { duration: 150 }
        }
    }

    Image {
        width: 0.05 * root.width; height: root.height
        source: "images/flick-left.svg"
        sourceSize { width: width; height: height }
        fillMode: Image.PreserveAspectFit
        opacity: flickable.atXBeginning ? 0 : 1
    }

    Flickable {
        id: flickable

        width: 0.9 * root.width
        height: parent.height
        contentWidth: pieceList.columns * root.pieceAreaSize
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        PieceList {
            id: pieceList

            rows: root.rows
            columns: Math.ceil(pieces.length / root.rows)
            pieceAreaSize: root.pieceAreaSize
            pieces: root.pieces
            onPiecePicked: root.piecePicked(piece)
        }

    }

    Image {
        width: 0.05 * root.width; height: root.height
        source: "images/flick-right.svg"
        sourceSize { width: width; height: height }
        fillMode: Image.PreserveAspectFit
        opacity: flickable.atXEnd ? 0 : 1
    }
}
