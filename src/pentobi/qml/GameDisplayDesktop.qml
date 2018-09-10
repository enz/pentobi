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
    id: root

    property Item pickedPiece

    // Values: "last_dot", "last_number", "all_number", "none"
    property string moveMarking: "last_dot"

    property alias showCoordinates: board.showCoordinates
    property bool enableAnimations: true
    property real animationDurationMove: enableAnimations ? 300 : 0
    property real animationDurationFast: enableAnimations ? 80 : 0
    property bool setupMode

    property size imageSourceSize: {
        var width = board.gridWidth, height = board.gridHeight
        if (board.isTrigon || board.isGembloQ)
            return Qt.size(2 * width, height)
        if (board.isNexos)
            return Qt.size(1.5 * width, 0.5 * height)
        if (board.isCallisto)
            return Qt.size(0.9 * width, 0.9 * height)
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

    signal play(var pieceModel, point gameCoord)

    function createPieces() { Logic.createPieces() }
    function destroyPieces() { Logic.destroyPieces() }
    function findPiece(pieceModel) { return Logic.findPiece(pieceModel) }
    function pickPieceAtBoard(piece) { Logic.pickPieceAtBoard(piece) }
    function shiftPiece(dx, dy) { Logic.shiftPiece(dx, dy) }
    function shiftPieceFast(dx, dy) { Logic.shiftPieceFast(dx, dy) }
    function playPickedPiece() { Logic.playPickedPiece() }
    function showToPlay() { }
    function showComment() { pageIndicator.currentIndex = 0 }
    function showPieces() { }
    function switchView() {
        if (analyzeGame.item)
            pageIndicator.currentIndex = (pageIndicator.currentIndex + 1) % 2
    }
    function dropCommentFocus() { comment.dropFocus() }
    function showMove(move) { Logic.showMove(move) }
    function getBoard() { return board }
    function showTemporaryMessage(text) {
        showStatus(text)
        messageTimer.restart()
    }
    function startSearch() { showStatus(qsTr("Computer is thinking…")) }
    function endSearch() { if (! messageTimer.running) clearStatus() }
    function startAnalysis() {
        pageIndicator.currentIndex = 1
        showStatus(qsTr("Running game analysis…"))
    }
    function endAnalysis() { if (! messageTimer.running) clearStatus() }
    function analysisAutoloaded() { pageIndicator.currentIndex = 1 }
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
                qsTr("Computer is thinking… (up to %1 seconds remaining)")
                .arg(seconds);
        else
        {
            var minutes = Math.ceil(remainingSeconds / 60);
            text =
                qsTr("Computer is thinking… (up to %1 minutes remaining)")
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

    onWidthChanged: Logic.dropPieceFast()
    onHeightChanged: Logic.dropPieceFast()

    Settings {
        property alias enableAnimations: root.enableAnimations
        property alias moveMarking: root.moveMarking
        property alias showCoordinates: root.showCoordinates
        property alias setupMode: root.setupMode

        category: "GameDisplayDesktop"
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            id: mainContent

            Layout.fillWidth: true
            Layout.fillHeight: true

            Row {
                id: row

                property real relativeBoardWidth: 0.52
                property real minSpacing: 5

                leftPadding: 5
                rightPadding: 5
                anchors.centerIn: parent
                spacing:
                    Math.min(
                        Math.max(minSpacing,
                                 mainContent.width - board.width
                                 - rightColumn.width - leftPadding
                                 - rightPadding),
                        0.04 * board.width)

                Board {
                    id: board

                    width: Math.min(row.relativeBoardWidth
                                    * (mainContent.width - row.leftPadding - row.rightPadding - row.minSpacing),
                                    mainContent.height)
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
                ColumnLayout {
                    id: rightColumn

                    width: board.width * (1 / row.relativeBoardWidth - 1)
                    height: board.grabImageTarget.height
                    anchors.verticalCenter: board.verticalCenter
                    spacing: 0

                    ScoreDisplay {
                        id: scoreDisplay

                        Layout.fillWidth: true
                        Layout.preferredHeight: 0.04 * parent.width
                    }
                    PieceSelectorDesktop {
                        id: pieceSelector

                        transitionsEnabled: false
                        Layout.topMargin:0.02 * width
                        Layout.fillWidth: true
                        Layout.preferredHeight:
                            (board.isTrigon ? 0.7 : 0.75) * parent.width
                        onPiecePicked: Logic.pickPiece(piece)
                    }
                    ColumnLayout {
                        spacing: analyzeGame.item ? 0.1 * font.pixelSize : 0
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.topMargin:0.01 * width

                        Item {
                            visible: analyzeGame.item
                            implicitHeight: pageIndicator.height
                            Layout.fillWidth: true

                            PageIndicator {
                                id: pageIndicator

                                anchors.centerIn: parent
                                count: 2
                                interactive: true
                                delegate: Rectangle {
                                    implicitWidth: 0.65 * font.pixelSize
                                    implicitHeight: implicitWidth
                                    radius: width / 2
                                    color: theme.colorText
                                    opacity:
                                        index === pageIndicator.currentIndex ?
                                            0.5 : pressed ? 0.2 : 0.15
                                }
                                onCurrentIndexChanged: dropCommentFocus()
                            }
                        }
                        Control {
                            Layout.fillHeight: true
                            Layout.fillWidth: true

                            Comment {
                                id: comment

                                visible: pageIndicator.currentIndex === 0
                                anchors.fill: parent
                            }
                            Loader {
                                id: analyzeGame

                                visible: pageIndicator.currentIndex === 1
                                anchors.fill: parent
                                sourceComponent:
                                    analyzeGameModel.elements.length > 0
                                    || analyzeGameModel.isRunning ?
                                        analyzeGameComponent : null
                                onSourceComponentChanged:
                                    if (! sourceComponent)
                                        showComment()

                                Component {
                                    id: analyzeGameComponent

                                    AnalyzeGame { theme: rootWindow.theme }
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

            Label {
                id: statusText

                opacity: 0
                color: theme.colorText
                leftPadding: 0.4 * font.pixelSize
                padding: 0.2 * font.pixelSize

                Behavior on opacity {
                    NumberAnimation {
                        duration: animationDurationFast
                    }
                }
            }
            Item { Layout.fillWidth: true }
            Label {
                text: gameModel.positionInfoShort
                color: theme.colorText
                opacity: 0.8
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
        width: {
            var f
            if (board.isTrigon) f = 7
            else if (board.isNexos) f = 11
            else if (board.isGembloQ) f = 10.5
            else if (board.isCallisto) f = 6.5
            else f = 7.3
            return Math.max(200, f * board.gridHeight)
        }
        height: width
        pieceModel: pickedPiece ? pickedPiece.pieceModel : null
        onPiecePlayed: Logic.playPickedPiece()
    }
}
