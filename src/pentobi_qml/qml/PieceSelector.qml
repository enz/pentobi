//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceSelector.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.1
import QtQml.Models 2.1

Item {
    id: root

    property real pieceAreaSize
    property int toPlay
    property var pieces0
    property var pieces1
    property var pieces2
    property var pieces3
    property int nuColors
    property int rows: 1

    signal piecePicked(var piece)

    ObjectModel {
        id: objectModel

        PieceListFlickable {
            width: root.width
            height: root.height
            rows: root.rows
            pieces: root.pieces0
            pieceAreaSize: root.pieceAreaSize
            onPiecePicked: root.piecePicked(piece)
        }
        PieceListFlickable {
            width: root.width
            height: root.height
            rows: root.rows
            pieces: root.pieces1
            pieceAreaSize: root.pieceAreaSize
            onPiecePicked: root.piecePicked(piece)
        }
        PieceListFlickable {
            visible: nuColors >= 3
            width: root.width
            height: visible ? root.height : 0
            rows: root.rows
            pieces: root.pieces2
            pieceAreaSize: visible ? root.pieceAreaSize : 0
            onPiecePicked: root.piecePicked(piece)
        }
        PieceListFlickable {
            visible: nuColors >= 4
            width: root.width
            height: visible ? root.height : 0
            rows: root.rows
            pieces: root.pieces3
            pieceAreaSize: visible ? root.pieceAreaSize : 0
            onPiecePicked: root.piecePicked(piece)
        }
    }

    ListView {
        id: listView

        model: objectModel
        anchors.fill: root
        clip: true
        snapMode: ListView.SnapToItem
        currentIndex: toPlay
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 100
        preferredHighlightBegin: 0
        preferredHighlightEnd: height
        highlightRangeMode: ListView.StrictlyEnforceRange
    }
}
