//-----------------------------------------------------------------------------
/** @file pentobi/qml/GameDisplayDesktop.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import Qt.labs.settings 1.0
import "." as Pentobi
import "GameDisplay.js" as Logic

Item
{
    id: gameDisplay // Referenced by Piece*.qml

    property Item pickedPiece

    // Values: "last_dot", "last_number", "all_number", "none"
    property string moveMarking: "last_dot"

    property QtObject theme
    property alias showCoordinates: board.showCoordinates
    property bool enableAnimations: true
    property bool setupMode

    // Dummy for compatibility with GameDisplayMobile
    property bool busyIndicatorRunning

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
    function showToPlay() { }
    function showAnalyzeGame() { }
    function showComment() { }
    function showPieces() { }
    function dropCommentFocus() { comment.focus = false }
    function showMove(move) { Logic.showMove(move) }
    function getBoard() { return board }
    function showTemporaryMessage(text) {
        showStatus(text)
        messageTimer.restart()
    }
    function startSearch() { showStatus(qsTr("Computer is thinking...")) }
    function endSearch() { if (! messageTimer.running) clearStatus() }
    function startAnalysis() { showStatus(qsTr("Running game analysis...")) }
    function endAnalysis() { if (! messageTimer.running) clearStatus() }
    function searchCallback(elapsedSeconds, remainingSeconds) {
        // If the search is longer than 10 sec, we show the (maximum) remaining
        // time (only during a move generation, ignore search callbacks during
        // game analysis)
        if (! playerModel.isGenMoveRunning || elapsedSeconds < 10)
            return;
        var text;
        var seconds = Math.ceil(remainingSeconds);
        if (seconds < 90)
            text =
                qsTr("Computer is thinking... (up to %1 seconds remaining)")
                .arg(seconds);
        else
        {
            var minutes = Math.ceil(remainingSeconds / 60);
            text =
                qsTr("Computer is thinking... (up to %1 minutes remaining)")
                .arg(minutes);
        }
        showStatus(text)
    }
    function openBoardContextMenu(moveNumber, x, y) {
        if (! boardContextMenu.item)
            boardContextMenu.sourceComponent = boardContextMenuComponent
        boardContextMenu.item.moveNumber = moveNumber
        if (isDesktop)
            boardContextMenu.item.popup()
        else
            boardContextMenu.item.popup(x, y)
    }

    function showStatus(text) {
        messageTimer.stop()
        statusText.text = text
        statusText.opacity = 1
    }
    function clearStatus() { statusText.opacity = 0 }

    onWidthChanged: pickedPiece = null
    onHeightChanged: pickedPiece = null

    Settings {
        property alias enableAnimations: gameDisplay.enableAnimations
        property alias moveMarking: gameDisplay.moveMarking
        property alias showCoordinates: gameDisplay.showCoordinates
        property alias setupMode: gameDisplay.setupMode
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Row {
                id: row

                property real relativeBoardWidth: 0.52

                width: Math.min(parent.width - leftPadding - rightPadding, parent.height / relativeBoardWidth)
                height: Math.min(parent.height, (parent.width - leftPadding - rightPadding) * relativeBoardWidth)
                leftPadding: spacing
                rightPadding: spacing
                anchors.centerIn: parent
                spacing: 5

                Board {
                    id: board

                    width: row.relativeBoardWidth * parent.width
                    height: isTrigon ? Math.sqrt(3) / 2 * width : width
                    anchors.verticalCenter: parent.verticalCenter
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
                Column {
                    id: rightColumn

                    width: parent.width - board.width - row.spacing - row.leftPadding - row.rightPadding
                    height: board.rows * board.gridHeight
                    anchors.verticalCenter: board.verticalCenter
                    spacing: 0.02 * width

                    ScoreDisplay {
                        id: scoreDisplay

                        height: 0.05 * parent.width
                        pointSize: 0.6 * height
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    PieceSelectorDesktop {
                        id: pieceSelector

                        width: parent.width
                        height: 0.67 * parent.height
                        transitionsEnabled: false
                        onPiecePicked: Logic.pickPiece(piece)
                    }
                    Column {
                        width: parent.width
                        height: parent.height - scoreDisplay.height - pieceSelector.height - 2 * parent.spacing

                        Comment {
                            id: comment

                            width: parent.width
                            height: analyzeGame.item ? 0.5 * (parent.height - parent.spacing) : parent.height
                        }
                        Loader {
                            id: analyzeGame

                            sourceComponent:
                                analyzeGameModel.elements.length > 0
                                || analyzeGameModel.isRunning ?
                                    analyzeGameComponent : null
                            width: parent.width
                            height: 0.5 * (parent.height - parent.spacing)

                            Component {
                                id: analyzeGameComponent

                                AnalyzeGame {
                                    theme: rootWindow.theme
                                }
                            }
                        }
                    }
                }
            }
        }

        // Status bar
        RowLayout {
            height: 1.5 * statusText.font.pixelSize
            Layout.fillWidth: true

            Text {
                id: statusText

                opacity: 0
                color: theme.colorText
                leftPadding: 0.4 * font.pixelSize
                padding: 0.2 * font.pixelSize

                Behavior on opacity { NumberAnimation { duration: 110 } }
            }
            Item { Layout.fillWidth: true }
            Text {
                id: posInfoText

                text: gameModel.positionInfoShort
                color: theme.colorText
                rightPadding: 0.4 * font.pixelSize
                padding: 0.2 * font.pixelSize
            }
            Timer {
                id: messageTimer

                interval: 3000
                onTriggered: clearStatus()
            }
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
        isOutsideBoard:
            ! board.contains(parent.mapToItem(board, x + width / 2,
                                              y + height / 2))
        width: (board.isNexos ? 12.5 : board.isGembloQ ? 12.1 : 8.5) * board.gridHeight; height: width
        visible: pickedPiece !== null
        pieceModel: pickedPiece !== null ? pickedPiece.pieceModel : null
        onPiecePlayed: Logic.playPickedPiece()
    }
}
