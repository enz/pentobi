//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/GameDisplay.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.1
import QtQuick.Window 2.1
import "GameDisplay.js" as Logic

Item
{
    id: root

    property var pickedPiece: null
    property bool transitionsEnabled: true

    // Location of the piece selector (for centering items like the busy cursor
    // on it)
    readonly property real pieceAreaSize: pieceSelector.pieceAreaSize
    readonly property real pieceSelectorY: column.y + pieceSelector.y
    readonly property real pieceSelectorHeight: pieceSelector.height

    property var _pieces0
    property var _pieces1
    property var _pieces2
    property var _pieces3
    property bool _isTrigon: boardModel.gameVariant.indexOf("trigon") >= 0

    signal play(var pieceModel, point gameCoord)

    function createPieces() { Logic.createPieces() }
    function showPieces(color) { pieceSelector.showColor(color) }
    function showMoveHint(move) { Logic.showMoveHint(move) }

    onWidthChanged: pickedPiece = null
    onHeightChanged: pickedPiece = null

    Background { anchors.fill: root }
    Column {
        id: column

        width: root.width
        anchors.centerIn: root
        spacing: 0.01 * board.width

        Board {
            id: board

            gameVariant: boardModel.gameVariant
            width: Math.min(parent.width,
                            (_isTrigon ? 0.9 : 0.8) * root.height)
            height: _isTrigon ? Math.sqrt(3) / 2 * width : width
            anchors.horizontalCenter: parent.horizontalCenter
        }
        ScoreDisplay {
            id: scoreDisplay

            gameVariant: boardModel.gameVariant
            points0: boardModel.points0
            points1: boardModel.points1
            points2: boardModel.points2
            points3: boardModel.points3
            hasMoves0: boardModel.hasMoves0
            hasMoves1: boardModel.hasMoves1
            hasMoves2: boardModel.hasMoves2
            hasMoves3: boardModel.hasMoves3
            toPlay: boardModel.isGameOver ? -1 : boardModel.toPlay
            height: 0.06 * board.width
            pointSize: 0.03 * board.width
            anchors.horizontalCenter: parent.horizontalCenter
        }
        PieceSelector {
            id: pieceSelector

            property int nuVisibleColumns: 7

            // Piece area size if no flickable indicators are needed
            property real _maxPieceAreaSize: board.width / nuVisibleColumns

            property int _maxRows: _isTrigon ? 4 : 3

            allPiecesFitInVisible:
                boardModel.nuPieces <= nuVisibleColumns * rows
            pieces0: _pieces0
            pieces1: _pieces1
            pieces2: _pieces2
            pieces3: _pieces3
            nuPiecesLeft0: boardModel.nuPiecesLeft0
            nuPiecesLeft1: boardModel.nuPiecesLeft1
            nuPiecesLeft2: boardModel.nuPiecesLeft2
            nuPiecesLeft3: boardModel.nuPiecesLeft3
            toPlay: boardModel.toPlay
            nuColors: boardModel.nuColors
            width: board.width
            anchors.horizontalCenter: board.horizontalCenter

            // Take into account that the effective visible width of the piece
            // list is only 94% of the piece selector width because of the
            // images indicating that the list is flickable.
            pieceAreaSize: (allPiecesFitInVisible ? 1 : 0.94) *
                           board.width / nuVisibleColumns

            rows: {
                if (_maxPieceAreaSize == 0)
                    return 1
                var height = root.height - board.height - scoreDisplay.height
                var rows = Math.floor(height / _maxPieceAreaSize)
                if (rows == 0)
                    return 1
                if (rows >= _maxRows)
                    return _maxRows
                return rows
            }

            height: rows * pieceAreaSize
            onPiecePicked: Logic.pickPiece(piece)
        }
    }

    PieceManipulator {
        id: pieceManipulator

        function isLegal() {
            if (pickedPiece === null)
                return false
            var boardCoord = parent.mapToItem(board,
                                              x + width / 2, y + height / 2)
            var coord = board.mapToGame(boardCoord.x, boardCoord.y)
            return boardModel.isLegalPos(pickedPiece.pieceModel, coord)
        }

        width: 0.6 * board.width
        z: 1
        visible: pickedPiece !== null
        pieceModel: pickedPiece !== null ? pickedPiece.pieceModel : null
        drag {
            minimumX: -width / 2; maximumX: root.width - width / 2
            minimumY: -height / 2; maximumY: root.height - height / 2
        }
        onPiecePlayed: {
            var pos = mapToItem(board, width / 2, height / 2)
            if (board.contains(Qt.point(pos.x, pos.y))) {
                if (isLegal()) {
                    var boardCoord =
                            pieceManipulator.mapToItem(board, pickedPiece.x, pickedPiece.y)
                    var coord = board.mapToGame(boardCoord.x, boardCoord.y)
                    play(pickedPiece.pieceModel, coord)
                    pickedPiece = null
                }
            }
            else
                pickedPiece = null
        }
        onXChanged: legal = isLegal()
        onYChanged: legal = isLegal()
    }
}
