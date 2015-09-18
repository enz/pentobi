import QtQuick 2.0
import QtQuick.Controls 1.1
import "GameDisplay.js" as Logic

Rectangle
{
    id: root

    property var pickedPiece: null
    property bool transitionsEnabled
    property alias busyIndicatorRunning: busyIndicator.running
    property alias pieceAreaSize: pieceSelector.pieceAreaSize
    property real imageSourceWidth:
        _isTrigon ? 2 * board.gridElementWidth : board.gridElementWidth
    property alias imageSourceHeight: board.gridElementHeight

    property var _pieces0
    property var _pieces1
    property var _pieces2
    property var _pieces3
    property bool _isTrigon: gameModel.gameVariant.indexOf("trigon") >= 0
    signal play(var pieceModel, point gameCoord)

    function createPieces() { Logic.createPieces() }
    function destroyPieces() { Logic.destroyPieces() }
    function showMoveHint(move) { Logic.showMoveHint(move) }
    function showToPlay() { pieceSelector.showToPlay() }
    function showPiecesImmediately(color) {
        pieceSelector.showColorImmediately(color)
    }
    function findPiece(pieceModel) { return Logic.findPiece(pieceModel) }

    color: theme.backgroundColor
    onWidthChanged: pickedPiece = null
    onHeightChanged: pickedPiece = null

    Column {
        id: column

        width: root.width
        anchors.centerIn: root
        spacing: 0.01 * board.width

        Board {
            id: board

            gameVariant: gameModel.gameVariant
            width: Math.min(parent.width,
                            (_isTrigon ? 0.9 : 0.8) * root.height)
            height: _isTrigon ? Math.sqrt(3) / 2 * width : width
            anchors.horizontalCenter: parent.horizontalCenter
        }
        ScoreDisplay {
            id: scoreDisplay

            gameVariant: gameModel.gameVariant
            points0: gameModel.points0
            points1: gameModel.points1
            points2: gameModel.points2
            points3: gameModel.points3
            hasMoves0: gameModel.hasMoves0
            hasMoves1: gameModel.hasMoves1
            hasMoves2: gameModel.hasMoves2
            hasMoves3: gameModel.hasMoves3
            toPlay: gameModel.isGameOver ? -1 : gameModel.toPlay
            altPlayer: gameModel.altPlayer
            height: board.gridElementHeight
            pointSize: 0.6 * height
            anchors.horizontalCenter: parent.horizontalCenter
        }
        PieceSelector {
            id: pieceSelector

            property int nuVisibleColumns: 7
            property int _maxRows:
                _isTrigon || gameModel.gameVariant == "junior" ? 4 : 3

            pieces0: _pieces0
            pieces1: _pieces1
            pieces2: _pieces2
            pieces3: _pieces3
            nuPiecesLeft0: gameModel.nuPiecesLeft0
            nuPiecesLeft1: gameModel.nuPiecesLeft1
            nuPiecesLeft2: gameModel.nuPiecesLeft2
            nuPiecesLeft3: gameModel.nuPiecesLeft3
            toPlay: gameModel.toPlay
            nuColors: gameModel.nuColors
            width: board.width
            anchors.horizontalCenter: board.horizontalCenter

            // Take into account that the effective visible width of the piece
            // list is only 94% of the piece selector width because of the
            // images indicating that the list is flickable.
            pieceAreaSize: 0.94 * board.width / nuVisibleColumns

            rows: {
                if (pieceAreaSize == 0)
                    return 1
                var height = root.height - board.height - scoreDisplay.height
                var rows = Math.floor(height / pieceAreaSize)
                if (rows == 0)
                    return 1
                if (rows >= _maxRows)
                    return _maxRows
                return rows
            }

            height: rows * pieceAreaSize
            onPiecePicked: Logic.pickPiece(piece)

            Loader {
                id: messageLoader

                anchors.centerIn: parent
            }
        }
    }
    BusyIndicator {
        id: busyIndicator

        x: (root.width - width) / 2
        y: column.y + pieceSelector.y + (pieceSelector.height - height) / 2
    }
    PieceManipulator {
        id: pieceManipulator

        function isLegal() {
            if (pickedPiece === null)
                return false
            var boardCoord = parent.mapToItem(board,
                                              x + width / 2, y + height / 2)
            var coord = board.mapToGame(boardCoord.x, boardCoord.y)
            return gameModel.isLegalPos(pickedPiece.pieceModel, coord)
        }

        width: 0.6 * board.width
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
        onOrientationChanged: legal = isLegal()
    }
}
