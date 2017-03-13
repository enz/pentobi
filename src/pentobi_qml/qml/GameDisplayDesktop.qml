import QtQuick 2.0
import QtQuick.Controls 1.1
import Qt.labs.controls 1.0 as Controls2
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import "GameDisplay.js" as Logic

Item
{
    id: gameDisplay // Referenced by Piece*.qml

    property var pickedPiece: null

    // Values: "last_dot", "last_number", "all_number", "none"
    property string moveMarking: "last_dot"

    property QtObject theme
    property alias showCoordinates: board.showCoordinates
    property bool enableAnimations: true
    property bool setupMode
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
    property alias pieces0: pieceSelector.pieces0
    property alias pieces1: pieceSelector.pieces1
    property alias pieces2: pieceSelector.pieces2
    property alias pieces3: pieceSelector.pieces3

    signal play(var pieceModel, point gameCoord)

    function createPieces() { Logic.createPieces() }
    function destroyPieces() { Logic.destroyPieces() }
    function showToPlay() { }
    function showAnalyzeGame() { pickedPiece = null; swipeView.currentIndex = 2 }
    function showComment() { pickedPiece = null; swipeView.currentIndex = 1 }
    function showPieces() { }
    function dropCommentFocus() { if (comment.activeFocus) forceActiveFocus() }
    function showMove(move) { Logic.showMove(move) }
    function getBoard() { return board }
    function showTemporaryMessage(text) { message.showTemporary(text) }

    onWidthChanged: pickedPiece = null
    onHeightChanged: pickedPiece = null

    Settings {
        property alias enableAnimations: gameDisplay.enableAnimations
        property alias moveMarking: gameDisplay.moveMarking
        property alias showCoordinates: gameDisplay.showCoordinates
        property alias setupMode: gameDisplay.setupMode
    }
    Row {
        id: column

        width: gameDisplay.width
        anchors.centerIn: gameDisplay
        spacing: 0.01 * board.width

        Board {
            id: board

            gameVariant: gameModel.gameVariant

            width: parent.width *0.52
            height: isTrigon ? Math.sqrt(3) / 2 * width : width
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if (! setupMode) return
                var mv = gameModel.addEmpty(pos)
                showMove(mv)
            }
        }
        Column {
            width: 0.9 * board.width
            anchors.verticalCenter: board.verticalCenter
            spacing: 0.02 * board.width

            ScoreDisplay {
                id: scoreDisplay

                height: 0.04 * board.width
                pointSize: 0.6 * height
                anchors.horizontalCenter: parent.horizontalCenter
            }
            PieceSelectorDesktop {
                id: pieceSelector

                width: parent.width
                height: width / columns * 8
                anchors.horizontalCenter: parent.horizontalCenter
                onPiecePicked: Logic.pickPiece(piece)
            }
            Controls2.SwipeView {
                id: swipeView

                width: parent.width
                height: 0.2 * width
                anchors.horizontalCenter: parent.horizontalCenter
                clip: true

                Comment {
                    id: comment
                }
                AnalyzeGame {
                    theme: root.theme
                }
            }
        }
    }
    BusyIndicator {
        id: busyIndicator

        x: (gameDisplay.width - width) / 2
        y: (gameDisplay.height - height) / 2
    }
    Rectangle {
        id: message

        function showTemporary(text) {
            messageText.text = text
            opacity = 1
            messageTimer.restart()
        }

        opacity: 0
        x: (gameDisplay.width - width) / 2
        y: (gameDisplay.height - height) / 2
        radius: 0.1 * height
        color: theme.messageBackgroundColor
        implicitWidth: messageText.implicitWidth + 0.5 * messageText.implicitHeight
        implicitHeight: 1.5 * messageText.implicitHeight

        Behavior on opacity { NumberAnimation { duration: 200 } }

        Text {
            id: messageText

            anchors.centerIn: parent
            color: theme.messageTextColor
        }
        Timer {
            id: messageTimer

            interval: 2500
            onTriggered: message.opacity = 0
        }
    }
    PieceManipulator {
        id: pieceManipulator

        innerSize: {
            if (board.isTrigon) return board.gridHeight * 6.2
            if (board.isNexos) return board.gridHeight * 8.9
            if (board.isCallisto) return board.gridHeight * 5.5
            if (board.isGembloQ) return board.gridHeight * 9.0
            return board.gridHeight * 6.5
        }
        legal: {
            if (pickedPiece === null) return false
            // Need explicit dependencies on x, y, pieceModel.state
            var pos = parent.mapToItem(board, x + width / 2, y + height / 2)
            if (setupMode)
                return gameModel.isLegalSetupPos(pickedPiece.pieceModel,
                                                 pickedPiece.pieceModel.state,
                                                 board.mapToGame(pos))
            return gameModel.isLegalPos(pickedPiece.pieceModel,
                                        pickedPiece.pieceModel.state,
                                        board.mapToGame(pos))
        }
        visible: pickedPiece !== null
        pieceModel: pickedPiece !== null ? pickedPiece.pieceModel : null
        onPiecePlayed: {
            var pos = mapToItem(board, width / 2, height / 2)
            if (! board.contains(Qt.point(pos.x, pos.y)))
                pickedPiece = null
            else if (setupMode)
                gameModel.addSetup(pieceModel, board.mapToGame(pos))
            else if (legal)
                play(pieceModel, board.mapToGame(pos))
        }
    }
}
