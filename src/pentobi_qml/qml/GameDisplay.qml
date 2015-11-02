import QtQuick 2.0
import QtQuick.Controls 1.1
import "GameDisplay.js" as Logic

Item
{
    id: gameDisplay // Referenced by Piece*.qml

    property var pickedPiece: null
    property bool transitionsEnabled
    property alias busyIndicatorRunning: busyIndicator.running
    property real imageSourceWidth:
        board.isTrigon ? 2 * board.gridElementWidth : board.gridElementWidth
    property alias imageSourceHeight: board.gridElementHeight

    property var _pieces0
    property var _pieces1
    property var _pieces2
    property var _pieces3
    signal play(var pieceModel, point gameCoord)

    function createPieces() { Logic.createPieces() }
    function destroyPieces() { Logic.destroyPieces() }
    function showMoveHint(move) { Logic.showMoveHint(move) }
    function showToPlay() { pieceSelector.showToPlay() }
    function showPiecesImmediately(color) {
        pieceSelector.showColorImmediately(color)
    }
    function findPiece(pieceModel, color) {
        return Logic.findPiece(pieceModel, color)
    }

    onWidthChanged: pickedPiece = null
    onHeightChanged: pickedPiece = null

    Column {
        id: column

        width: gameDisplay.width
        anchors.centerIn: gameDisplay
        spacing: 0.01 * board.width

        Board {
            id: board

            gameVariant: gameModel.gameVariant
            width: Math.min(parent.width,
                            gameDisplay.height
                            / (1 + 1 / board.rows
                               + pieceSelector.rows / pieceSelector.columns))
            height: isTrigon ? Math.sqrt(3) / 2 * width : width
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

            columns: board.rows <= 14 ? 7 : 8
            rows: 3
            pieces0: _pieces0
            pieces1: _pieces1
            pieces2: _pieces2
            pieces3: _pieces3
            toPlay: gameModel.toPlay
            nuColors: gameModel.nuColors
            width: board.width
            height: width / columns * rows
            anchors.horizontalCenter: board.horizontalCenter
            onPiecePicked: Logic.pickPiece(piece)
        }
    }
    BusyIndicator {
        id: busyIndicator

        x: (gameDisplay.width - width) / 2
        y: column.y + pieceSelector.y + (pieceSelector.height - height) / 2
    }
    PieceManipulator {
        id: pieceManipulator

        legal: {
            if (pickedPiece === null)
                return false
            // Don't use mapToItem(board, width / 2, height / 2), we want a
            // dependency on x, y.
            var pos = parent.mapToItem(board, x + width / 2, y + height / 2)
            return gameModel.isLegalPos(pickedPiece.pieceModel,
                                        pickedPiece.pieceModel.state,
                                        board.mapToGame(pos.x, pos.y))
        }
        width: 0.6 * board.width
        visible: pickedPiece !== null
        pieceModel: pickedPiece !== null ? pickedPiece.pieceModel : null
        drag {
            minimumX: -width / 2; maximumX: gameDisplay.width - width / 2
            minimumY: -height / 2; maximumY: gameDisplay.height - height / 2
        }
        onPiecePlayed: {
            var pos = mapToItem(board, width / 2, height / 2)
            if (! board.contains(Qt.point(pos.x, pos.y)))
                pickedPiece = null
            else if (legal) {
                play(pieceModel, board.mapToGame(pos.x, pos.y))
                pickedPiece = null
            }
        }
    }
}
