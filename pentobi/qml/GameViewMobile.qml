//-----------------------------------------------------------------------------
/** @file pentobi/qml/GameViewMobile.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import "." as Pentobi
import "GameView.js" as Logic

Item
{
    id: root

    property Item pickedPiece

    // Values: "last_dot", "last_number", "all_number", "none"
    property string moveMarking: "last_dot"

    property alias showCoordinates: board.showCoordinates
    property bool enableAnimations: true
    property real animationDurationMove: enableAnimations ? 300 : 0
    property real animationDurationFast: enableAnimations ? 80 : 0
    property bool setupMode
    property alias boardContextMenu: boardContextMenu
    property size imageSourceSize: {
        var width = board.gridWidth, height = board.gridHeight
        if (board.isTrigon || board.isGembloQ)
            return Qt.size(2 * width, height)
        if (board.isNexos)
            return Qt.size(1.5 * width, 1.5 * height)
        if (board.isCallisto)
            return Qt.size(0.95 * width, 0.95 * height)
        return Qt.size(width, height)
    }
    property alias pieces0: pieceSelector.pieces0
    property alias pieces1: pieceSelector.pieces1
    property alias pieces2: pieceSelector.pieces2
    property alias pieces3: pieceSelector.pieces3
    property var color0: {
        if (gameModel.gameVariant === "duo") return theme.colorPurple
        if (gameModel.gameVariant === "junior") return theme.colorGreen
        return theme.colorBlue
    }
    property var color1: {
        if (gameModel.gameVariant === "duo"
                || gameModel.gameVariant === "junior") return theme.colorOrange
        if (gameModel.nuColors === 2) return theme.colorGreen
        return theme.colorYellow
    }
    property var color2: theme.colorRed
    property var color3: theme.colorGreen
    property bool isCommentVisible: swipeView.currentIndex === 1
    property bool isPortrait: width <= height

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
    function searchCallback(elapsedSeconds, remainingSeconds) { }
    function startSearch() { }
    function endSearch() { }
    function startAnalysis() { showAnalyzeGame() }
    function endAnalysis() { }
    function deleteAnalysis() { if (swipeView.currentIndex === 2) showPieces() }
    function analysisAutoloaded() { }
    function openBoardContextMenu(moveNumber, x, y) {
        if (! boardContextMenu.item)
            boardContextMenu.sourceComponent = boardContextMenuComponent
        boardContextMenu.item.moveNumber = moveNumber
        if (isDesktop)
            boardContextMenu.item.popup()
        else
            boardContextMenu.item.popup(x, y)
    }

    onWidthChanged: Logic.dropPieceFast()
    onHeightChanged: Logic.dropPieceFast()

    Settings {
        property alias enableAnimations: root.enableAnimations
        property alias moveMarking: root.moveMarking
        property alias showCoordinates: root.showCoordinates
        property alias swipeViewCurrentIndex: swipeView.currentIndex
        property alias setupMode: root.setupMode

        category: "GameViewMobile"
    }
    Board {
        id: board

        x: isPortrait ? (parent.width - width) / 2 : 0
        width: isPortrait ? Math.min(parent.width, 0.7 * parent.height)
                          : parent.width / 2
        height: isPortrait ? width : parent.height
        onClicked: Logic.onBoardClicked(pos)
        onRightClicked: Logic.onBoardRightClicked(pos)

        Loader {
            id: boardContextMenu

            Component {
                id: boardContextMenuComponent

                BoardContextMenu { }
            }
        }
    }
    SwipeView {
        id: swipeView

        x: isPortrait ? (parent.width - board.width) / 2
                      : board.width + 0.02 * board.width
        y: isPortrait ? board.height + 0.01 * board.width
                      : board.grabImageTarget.y
        width: isPortrait ? board.width : parent.width - x
        height: isPortrait ? parent.height - y : parent.height - y
        clip: true

        Column {
            id: columnPieces

            spacing: 2

            ScoreDisplay {
                id: scoreDisplay

                width: swipeView.width
                height: 0.06 * swipeView.width
            }
            PieceSelectorMobile {
                id: pieceSelector

                property real elementSize:
                    // Show at least 3 rows
                    Math.min(parent.width / columns, height / 3)

                columns: pieces0 && pieces0.length <= 21 ? 7 : 8
                x: isPortrait ? (parent.width - width) / 2 : 0
                width: elementSize * columns
                height: swipeView.height - scoreDisplay.height
                        - columnPieces.spacing
                rowSpacing: {
                    // Don't show partial pieces
                    var n = Math.floor(height / elementSize)
                    return (height - n * elementSize) / n
                }
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
                Layout.margins: 0.01 * parent.width
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            NavigationButtons
            {
                Layout.fillWidth: true
                Layout.maximumHeight:
                    Math.min(50, 0.08 * rootWindow.contentItem.height,
                             root.width / 6)
            }
        }
    }
    Pentobi.BusyIndicator {
        id: busyIndicator

        running: busyIndicatorRunning
        width: Math.min(0.2 * swipeView.width, swipeView.height)
        height: width
        x: swipeView.x + (swipeView.width - width) / 2
        y: swipeView.y + (swipeView.height - height) / 2
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
        x: swipeView.x + (swipeView.width - width) / 2
        y: swipeView.y + (swipeView.height - height) / 2
        radius: 0.1 * height
        color: theme.colorMessageBase
        implicitWidth: messageText.implicitWidth + 0.5 * messageText.implicitHeight
        implicitHeight: 1.5 * messageText.implicitHeight

        Behavior on opacity {
            NumberAnimation {
                duration: animationDurationFast
            }
        }

        Text {
            id: messageText

            anchors.centerIn: parent
            color: theme.colorMessageText
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
        width: {
            var f
            if (board.isTrigon) f = 7
            else if (board.isNexos) f = 12.5
            else if (board.isGembloQ) f = 12
            else if (board.isCallisto) f = 6.7
            else f = 8.7
            return Math.max(200, f * board.gridHeight)
        }
        height: width
        pieceModel: pickedPiece ? pickedPiece.pieceModel : null
        onPiecePlayed: Logic.playPickedPiece()
    }
}
