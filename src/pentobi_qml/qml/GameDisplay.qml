import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls 2.1 as Controls2
import QtQuick.Layouts 1.0
import "GameDisplay.js" as Logic

Item
{
    id: gameDisplay // Referenced by Piece*.qml

    property var pickedPiece: null

    // Values: "last_dot", "last_number", "all_number", "none"
    property string moveMarking: "last_dot"

    property alias showCoordinates: board.showCoordinates
    property bool enableAnimations: true
    property bool setupMode
    property alias busyIndicatorRunning: busyIndicator.running
    property alias swipeViewCurrentIndex: swipeView.currentIndex
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

    function changeGameVariant(gameVariant) {
        destroyPieces()
        gameModel.changeGameVariant(gameVariant)
        createPieces()
        showToPlay()
        setupMode = false
    }
    function createPieces() { Logic.createPieces() }
    function destroyPieces() { Logic.destroyPieces() }
    function newGame() {
        gameModel.newGame()
        setupMode = false
        showToPlay()
    }
    function showToPlay() { pieceSelector.contentY = 0 }
    function showAnalyzeGame() { pickedPiece = null; swipeViewCurrentIndex = 2 }
    function showComment() { pickedPiece = null; swipeViewCurrentIndex = 1 }
    function showPieces() { swipeViewCurrentIndex = 0 }
    function dropCommentFocus() {
        if (navigationPanel.activeFocusComment)
            forceActiveFocus()
    }
    function showMove(move) { Logic.showMove(move) }
    function grabBoardToImage(callback, width) {
        return board.grabToImage(callback,
                                 Qt.size(width,
                                         width * board.height / board.width))
    }
    function showTemporaryMessage(text) { message.showTemporary(text) }

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
            width: Math.min(
                       parent.width,
                       gameDisplay.height / (1.07 + 2.7 / pieceSelector.columns))
            height: isTrigon ? Math.sqrt(3) / 2 * width : width
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                if (! setupMode) return
                var mv = gameModel.addEmpty(pos)
                showMove(mv)
            }
        }
        Controls2.SwipeView {
            id: swipeView

            width: Math.min(gameDisplay.width, 1.4 * board.width)
            height: Math.min(gameDisplay.height - board.height, board.height)
            anchors.horizontalCenter: board.horizontalCenter

            Column {
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
                    height: 0.05 * board.width
                    pointSize: 0.6 * height
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                PieceSelector {
                    id: pieceSelector

                    columns: gameModel.gameVariant.startsWith("classic")
                             || gameModel.gameVariant.startsWith("callisto")
                             || gameModel.gameVariant === "duo" ? 7 : 8
                    // Use width such that height shows only full rows
                    width: columns * height / Math.ceil(height / (swipeView.width / columns))
                    height: swipeView.height - scoreDisplay.height
                    gameVariant: gameModel.gameVariant
                    toPlay: gameModel.toPlay
                    nuColors: gameModel.nuColors
                    transitionsEnabled: false
                    anchors.horizontalCenter: parent.horizontalCenter
                    onPiecePicked: Logic.pickPiece(piece)
                }
            }
            NavigationPanel {
                id: navigationPanel
            }
            ColumnLayout {
                AnalyzeGame {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                NavigationButtons
                {
                    height: width / 6
                    Layout.fillWidth: true
                }
            }
        }
    }
    BusyIndicator {
        id: busyIndicator

        x: (gameDisplay.width - width) / 2
        y: column.y + swipeView.y + (swipeView.height - height) / 2
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
        y: column.y + swipeView.y + (swipeView.height - height) / 2
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

        legal: {
            if (pickedPiece === null) return false
            // The following expressions need explicit dependencies on
            // x, y, pieceModel.state
            var pos = parent.mapToItem(board, x + width / 2, y + height / 2)
            if (setupMode)
                return gameModel.isLegalSetupPos(pickedPiece.pieceModel,
                                                 pickedPiece.pieceModel.state,
                                                 board.mapToGame(pos))
            return gameModel.isLegalPos(pickedPiece.pieceModel,
                                        pickedPiece.pieceModel.state,
                                        board.mapToGame(pos))
        }
        width: 0.6 * board.width; height: width
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
    Connections {
        target: gameModel
        onPositionChanged: {
            pickedPiece = null
            if (gameModel.canGoBackward || gameModel.canGoForward
                    || gameModel.moveNumber > 0)
                setupMode = false
            analyzeGameModel.markCurrentMove(gameModel)
            dropCommentFocus()
        }
    }
}
