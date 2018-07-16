import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import "." as Pentobi
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
        if (board.isTrigon || board.isGembloQ)
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
    function findPiece(pieceModel) { return Logic.findPiece(pieceModel) }
    function pickPieceAtBoard(piece) { Logic.pickPieceAtBoard(piece) }
    function shiftPiece(dx, dy) { Logic.shiftPiece(dx, dy) }
    function shiftPieceFast(dx, dy) { Logic.shiftPieceFast(dx, dy) }
    function playPickedPiece() { Logic.playPickedPiece() }
    function showToPlay() { pieceSelector.contentY = 0 }
    function showAnalyzeGame() { pickedPiece = null; swipeView.currentIndex = 2 }
    function showComment() { pickedPiece = null; swipeView.currentIndex = 1 }
    function showPieces() { swipeView.currentIndex = 0 }
    function dropCommentFocus() { navigationPanel.dropCommentFocus() }
    function showMove(move) { Logic.showMove(move) }
    function getBoard() { return board }
    function showTemporaryMessage(text) { message.showTemporary(text) }

    // For compatibility with GameDisplayDesktop
    function searchCallback(elapsedSeconds, remainingSeconds) { }
    function startSearch() { }
    function endSearch() { }

    onWidthChanged: pickedPiece = null
    onHeightChanged: pickedPiece = null

    Settings {
        property alias enableAnimations: gameDisplay.enableAnimations
        property alias moveMarking: gameDisplay.moveMarking
        property alias showCoordinates: gameDisplay.showCoordinates
        property alias swipeViewCurrentIndex: swipeView.currentIndex
        property alias setupMode: gameDisplay.setupMode
    }
    Column {
        id: column

        width: gameDisplay.width
        anchors.centerIn: gameDisplay
        spacing: 0.01 * board.width

        Board {
            id: board

            // Ensures that at least 3 rows are shown in pieceSelector
            width: Math.min(
                       parent.width,
                       gameDisplay.height / (1.05 + 0.85 / pieceSelector.columns * 3))

            height: isTrigon ? Math.sqrt(3) / 2 * width : width
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                dropCommentFocus()
                if (! setupMode) return
                var mv = gameModel.addEmpty(pos)
                showMove(mv)
            }
        }
        SwipeView {
            id: swipeView

            width: board.width
            height: Math.min(gameDisplay.height - board.height, board.height)
            clip: true
            anchors.horizontalCenter: board.horizontalCenter

            Column {
                ScoreDisplay {
                    id: scoreDisplay

                    height: 0.05 * board.width
                    pointSize: 0.6 * height
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                PieceSelectorMobile {
                    id: pieceSelector

                    // How many full rows can we show if we use 85% of the board width?
                    property int rows: Math.floor(height / (0.85 * swipeView.width / columns))

                    columns: gameModel.gameVariant.startsWith("classic")
                             || gameModel.gameVariant.startsWith("callisto")
                             || gameModel.gameVariant === "duo" ? 7 : 8

                    // Show at least 1 row
                    width: rows < 1 ? columns * height : 0.85 * swipeView.width

                    height: swipeView.height - scoreDisplay.height

                    // Try not to show partial piece rows unless we cannot even
                    // show all pieces of one color (3 rows)
                    spacingPieceLists: rows < 3 ? 0 : height - rows * (width / columns)

                    anchors.horizontalCenter: parent.horizontalCenter
                    gameVariant: gameModel.gameVariant
                    toPlay: gameModel.toPlay
                    nuColors: gameModel.nuColors
                    transitionsEnabled: false
                    onPiecePicked: Logic.pickPiece(piece)
                }
            }
            NavigationPanel {
                id: navigationPanel
            }
            ColumnLayout {
                AnalyzeGame {
                    theme: rootWindow.theme
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                NavigationButtons
                {
                    Layout.fillWidth: true
                    Layout.preferredHeight: Math.min(implicitButtonWidth, parent.width / 6)
                }
            }
        }
    }
    Pentobi.BusyIndicator {
        id: busyIndicator

        width: Math.min(0.2 * swipeView.width, swipeView.height)
        height: width
        x: (gameDisplay.width - width) / 2
        y: column.y + swipeView.y + (swipeView.height - height) / 2
        opacity: 0.7
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
        width: 0.6 * board.width; height: width
        visible: pickedPiece !== null
        pieceModel: pickedPiece !== null ? pickedPiece.pieceModel : null
        onPiecePlayed: Logic.playPickedPiece()
    }
}
