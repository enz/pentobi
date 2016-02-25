import QtQuick 2.0
import QtQuick.Controls 1.1
import "GameDisplay.js" as Logic

Item
{
    id: gameDisplay // Referenced by Piece*.qml

    property var pickedPiece: null
    property bool transitionsEnabled
    property bool markLastMove: true
    property alias busyIndicatorRunning: busyIndicator.running
    property size imageSourceSize: {
        var width = board.gridWidth, height = board.gridHeight
        if (board.isTrigon)
            return Qt.size(2 * width, height)
        if (board.isNexos)
            return Qt.size(1.5 * width, 1.5 * height)
        if (board.isCallisto)
            return Qt.size(0.9 * width, 0.9 * height)
        return Qt.size(width, height)
    }
    property var _pieces0
    property var _pieces1
    property var _pieces2
    property var _pieces3

    signal play(var pieceModel, point gameCoord)

    function createPieces() { Logic.createPieces() }
    function destroyPieces() { Logic.destroyPieces() }
    function showToPlay() { pieceSelector.contentY = 0 }
    function showMoveHint(move) { Logic.showMoveHint(move) }
    function findPiece(pieceModel, color) {
        return Logic.findPiece(pieceModel, color)
    }
    function dropPiece() {
        pickedPiece = null
        transitionsEnabled = false
    }

    onWidthChanged: dropPiece()
    onHeightChanged: dropPiece()
    Component.onCompleted: {
        gameModel.positionChanged.connect(dropPiece)
    }

    Column {
        id: column

        width: gameDisplay.width
        anchors.centerIn: gameDisplay
        spacing: 0.01 * board.width

        Board {
            id: board

            gameVariant: gameModel.gameVariant
            width: Math.min(
                       parent.width,
                       gameDisplay.height / (1.07 + 2.7 / pieceSelector.columns))
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
            bonus0: gameModel.bonus0
            bonus1: gameModel.bonus1
            bonus2: gameModel.bonus2
            bonus3: gameModel.bonus3
            hasMoves0: gameModel.hasMoves0
            hasMoves1: gameModel.hasMoves1
            hasMoves2: gameModel.hasMoves2
            hasMoves3: gameModel.hasMoves3
            toPlay: gameModel.isGameOver ? -1 : gameModel.toPlay
            altPlayer: gameModel.altPlayer
            height: board.width / 20
            pointSize: 0.6 * height
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Flickable {
            id: flickable

            width: 0.9 * board.width
            height: width / pieceSelector.columns * pieceSelector.rows
            contentWidth: 2 * width
            contentHeight: height
            anchors.horizontalCenter: board.horizontalCenter
            clip: true
            onMovementEnded: {
                snapAnimation.to = contentX > width / 2 ? width : 0
                snapAnimation.restart()
            }

            Row {
                id: flickableContent

                PieceSelector {
                    id: pieceSelector

                    columns: gameModel.gameVariant.indexOf("classic") == 0
                             || gameModel.gameVariant.indexOf("callisto") == 0
                             || gameModel.gameVariant == "duo" ? 7 : 8
                    width: flickable.width
                    height: flickable.height
                    rows: 3
                    gameVariant: gameModel.gameVariant
                    pieces0: _pieces0
                    pieces1: _pieces1
                    pieces2: _pieces2
                    pieces3: _pieces3
                    toPlay: gameModel.toPlay
                    nuColors: gameModel.nuColors
                    onPiecePicked: Logic.pickPiece(piece)
                }
                NavigationPanel {
                    width: flickable.width
                    height: flickable.height
                }
            }
            SmoothedAnimation {
                id: snapAnimation

                target: flickable
                property: "contentX"
                duration: 200
            }
        }
    }
    BusyIndicator {
        id: busyIndicator

        x: (gameDisplay.width - width) / 2
        y: column.y + flickable.y + (flickable.height - height) / 2
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
        width: 0.6 * board.width; height: width
        visible: pickedPiece !== null
        pieceModel: pickedPiece !== null ? pickedPiece.pieceModel : null
        drag {
            minimumX: -width / 2; maximumX: gameDisplay.width - width / 2
            minimumY: -height / 2; maximumY: gameDisplay.height - height / 2
        }
        onPiecePlayed: {
            var pos = mapToItem(board, width / 2, height / 2)
            if (! board.contains(Qt.point(pos.x, pos.y)))
                dropPiece()
            else if (legal)
                play(pieceModel, board.mapToGame(pos.x, pos.y))
        }
    }
}
