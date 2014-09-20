//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/Main.js
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

function cancelGenMove() {
    playerModel.cancelGenMove()
    busyIndicator.running = false
}

function checkComputerMove() {
    checkGameFinsihed()
    if (! isComputerToPlay())
        return
    switch (boardModel.toPlay) {
    case 0: if (! boardModel.hasMoves0) return; break
    case 1: if (! boardModel.hasMoves1) return; break
    case 2: if (! boardModel.hasMoves2) return; break
    case 3: if (! boardModel.hasMoves3) return; break
    }
    genMove();
}

function checkGameFinsihed() {
    if (! boardModel.isGameOver)
        return
    var msg, points0, points1, points2, points3
    if (boardModel.nuColors == 2) {
        points0 = boardModel.points0
        points1 = boardModel.points1
        if (points0 > points1)
            msg = "Blue wins"
        else if (points0 < points1)
            msg = "Green wins"
        else
            msg = "Game ends in a tie"
    }
    else if (boardModel.nuPlayers == 4) {
        points0 = boardModel.points0
        points1 = boardModel.points1
        points2 = boardModel.points2
        points3 = boardModel.points3
        var maxPoints = Math.max(points0, points1, points2, points3)
        var nuWinners = 0
        if (points0 == maxPoints) ++nuWinners
        if (points1 == maxPoints) ++nuWinners
        if (points2 == maxPoints) ++nuWinners
        if (points3 == maxPoints) ++nuWinners
        if (nuWinners > 1)
            msg = "Game ends in a tie"
        else if (points0 == maxPoints)
            msg = "Blue wins"
        else if (points1 == maxPoints)
            msg = "Yellow wins"
        else if (points2 == maxPoints)
            msg = "Red wins"
        else if (points3 == maxPoints)
            msg = "Green wins"
    }
    else {
        points0 = boardModel.points0 + boardModel.points2
        points1 = boardModel.points1 + boardModel.points3
        if (points0 > points1)
            msg = "Blue/Red wins"
        else if (points0 < points1)
            msg = "Yellow/Green wins"
        else
            msg = "Game ends in a tie"
    }
    showMessage(msg)
}

/** If the computer already plays the current color to play, start generating
    a move; if he doesn't, make him play the current color (and only the
    current color). */
function computerPlay() {
    if (playerModel.isGenMoveRunning)
        return
    computerColorDialog.visible = false
    if (! isComputerToPlay()) {
        var isMultiColor = (boardModel.gameVariant == "classic_2"
                            || boardModel.gameVariant == "trigon_2")
        computerPlays0 = false
        computerPlays1 = false
        computerPlays2 = false
        computerPlays3 = false
        switch (boardModel.toPlay) {
        case 0:
            computerPlays0 = true
            if (isMultiColor) computerPlays2 = true
            break;
        case 1:
            computerPlays1 = true
            if (isMultiColor) computerPlays3 = true
            break;
        case 2:
            computerPlays2 = true
            if (isMultiColor) computerPlays0 = true
            break;
        case 3:
            computerPlays3 = true
            if (isMultiColor) computerPlays1 = true
            break;
        }
    }
    checkComputerMove()
}

function genMove() {
    cancelGenMove()
    gameDisplay.pickedPiece = null
    busyIndicator.running = true
    playerModel.startGenMove(boardModel)
}

function initComputerColors() {
    // Default setting is that the computer plays all colors but the first
    computerPlays0 = false
    computerPlays1 = true
    computerPlays2 = true
    computerPlays3 = true
    if (boardModel.gameVariant == "classic_2"
            || boardModel.gameVariant == "trigon_2")
        computerPlays2 = false
}

function initGameVariant(gameVariant) {
    cancelGenMove()
    computerColorDialog.visible = false
    message.clear()
    boardModel.initGameVariant(gameVariant)
    gameDisplay.createPieces()
    initComputerColors()
}

function isComputerToPlay() {
    switch (boardModel.toPlay) {
    case 0: return computerPlays0
    case 1: return computerPlays1
    case 2: return computerPlays2
    case 3: return computerPlays3
    }
}

function newGame()
{
    cancelGenMove()
    gameDisplay.pickedPiece = null
    message.clear()
    computerColorDialog.visible = false
    boardModel.newGame()
    initComputerColors()
}

function onComputerPlayed() {
    busyIndicator.running = false
    checkComputerMoveTimer.start()
}

function play(pieceModel, gameCoord) {
    cancelGenMove()
    boardModel.play(pieceModel, gameCoord)
    checkComputerMove()
}

function showComputerColorDialog() {
    computerColorDialog.computerPlays0 = root.computerPlays0
    computerColorDialog.computerPlays1 = root.computerPlays1
    computerColorDialog.computerPlays2 = root.computerPlays2
    computerColorDialog.computerPlays3 = root.computerPlays3
    computerColorDialog.visible = true
}

function showMessage(text) {
    message.show(text)
}

function showTemporaryMessage(text) {
    message.showTemporary(text, 1800)
}

function undo() {
    cancelGenMove()
    message.clear()
    boardModel.undo()
    gameDisplay.pickedPiece = null
}

function quit() {
    cancelGenMove()
    Qt.quit()
}
