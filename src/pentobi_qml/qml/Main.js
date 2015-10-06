function autosave() {
    if (boardModel.isGameOver)
        boardModel.clearAutoSave()
    else
        boardModel.autoSave()
}

function cancelGenMove() {
    playerModel.cancelGenMove()
    gameDisplay.busyIndicatorRunning = false
}

function changeGameVariant(gameVariant, verifyAbortGame) {
    if (boardModel.gameVariant === gameVariant)
        return
    cancelGenMove()
    if (! boardModel.isBoardEmpty && ! boardModel.isGameOver &&
            verifyAbortGame) {
        showMessageDialog(qsTr("New game?"),
                          function() {
                              changeGameVariant(gameVariant, false) })
        return
    }
    callDelayTimer.call(function() {
        initGameVariant(gameVariant)
        gameDisplay.transitionsEnabled = false
        initComputerColors()
        gameDisplay.showToPlay()
        gameDisplay.transitionsEnabled = true
    })
}

function checkComputerMove() {
    if (boardModel.isGameOver) {
        showGameOver()
        return
    }
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

function clearMarks() {
    if (_pieceMarked0 != null) {
        _pieceMarked0.isMarked = false
        _pieceMarked0 = null
    }
    if (_pieceMarked1 != null) {
        _pieceMarked1.isMarked = false
        _pieceMarked1 = null
    }
    if (_pieceMarked2 != null) {
        _pieceMarked2.isMarked = false
        _pieceMarked2 = null
    }
    if (_pieceMarked3 != null) {
        _pieceMarked3.isMarked = false
        _pieceMarked3 = null
    }
}

function clearMarksNotComputer() {
    if (! computerPlays0 && _pieceMarked0 != null) {
        _pieceMarked0.isMarked = false
        _pieceMarked0 = null
    }
    if (! computerPlays1 && _pieceMarked1 != null) {
        _pieceMarked1.isMarked = false
        _pieceMarked1 = null
    }
    if (! computerPlays2 && _pieceMarked2 != null) {
        _pieceMarked2.isMarked = false
        _pieceMarked2 = null
    }
    if (! computerPlays3 && _pieceMarked3 != null) {
        _pieceMarked3.isMarked = false
        _pieceMarked3 = null
    }
}

/** If the computer already plays the current color to play, start generating
    a move; if he doesn't, make him play the current color (and only the
    current color). */
function computerPlay() {
    if (playerModel.isGenMoveRunning)
        return
    hideComputerColorDialog()
    if (! isComputerToPlay()) {
        computerPlays0 = false
        computerPlays1 = false
        computerPlays2 = false
        computerPlays3 = false
        var variant = boardModel.gameVariant
        if (variant == "classic_3" && boardModel.toPlay == 3) {
            switch (boardModel.altPlayer) {
            case 0: computerPlays0 = true; break
            case 1: computerPlays1 = true; break
            case 2: computerPlays2 = true; break
            }
        }
        else
        {
            var isMultiColor =
                    (variant == "classic_2" || variant == "trigon_2")
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
    }
    checkComputerMove()
}

function computerPlays(color) {
    switch (color) {
    case 0: return computerPlays0
    case 1: return computerPlays1
    case 2: return computerPlays2
    case 3: return computerPlays3
    }
}

function computerPlaysAll() {
    switch (boardModel.gameVariant) {
    case "duo":
    case "junior":
    case "classic_2":
    case "trigon_2":
        return computerPlays0 && computerPlays1
    case "trigon_3":
        return computerPlays0 && computerPlays1 && computerPlays2
    default:
        return computerPlays0 && computerPlays1 && computerPlays2 &&
                computerPlays3
    }
}

function createTheme(themeName) {
    var source = "qrc:///qml/themes/" + themeName + "/Theme.qml"
    var component = Qt.createComponent(source)
    if (component.status != Component.Ready)
        throw "Could not load " + source
    return component.createObject(root)
}

function genMove() {
    cancelGenMove()
    gameDisplay.pickedPiece = null
    gameDisplay.busyIndicatorRunning = true
    gameDisplay.showToPlay()
    isMoveHintRunning = false
    playerModel.startGenMove(boardModel)
}

function hideComputerColorDialog()
{
    if (computerColorDialogLoader.status == Loader.Ready)
        computerColorDialogLoader.item.visible = false
}

function init() {
    callDelayTimer.call(function() {
        gameDisplay.createPieces()
        if (! boardModel.loadAutoSave())
            initComputerColors()
        else {
            clearMarks()
            markLastMove()
            if (! computerPlaysAll())
                checkComputerMove()
        }
        gameDisplay.transitionsEnabled = true
    })
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
    hideComputerColorDialog()
    gameDisplay.clearMessage()
    clearMarks()
    gameDisplay.destroyPieces()
    boardModel.initGameVariant(gameVariant)
    gameDisplay.createPieces()
}

function isComputerToPlay() {
    if (boardModel.gameVariant == "classic_3" && boardModel.toPlay == 3)
        return computerPlays(boardModel.altPlayer)
    return computerPlays(boardModel.toPlay)
}

function markLastMove() {
    if (! root.markLastMove)
        return
    var color = boardModel.getLastMoveColor()
    var piece = gameDisplay.findPiece(boardModel.getLastMovePieceModel(), color)
    if (piece === null)
        return
    switch (color) {
    case 0:
        if (_pieceMarked0 != null) _pieceMarked0.isMarked = false
        _pieceMarked0 = piece
        break
    case 1:
        if (_pieceMarked1 != null) _pieceMarked1.isMarked = false
        _pieceMarked1 = piece
        break
    case 2:
        if (_pieceMarked2 != null) _pieceMarked2.isMarked = false
        _pieceMarked2 = piece
        break
    case 3:
        if (_pieceMarked3 != null) _pieceMarked3.isMarked = false
        _pieceMarked3 = piece
        break
    }
    piece.isMarked = true
}

function moveGenerated(move) {
    gameDisplay.busyIndicatorRunning = false
    if (isMoveHintRunning) {
        gameDisplay.showMoveHint(move)
        isMoveHintRunning = false
        return
    }
    boardModel.playMove(move)
    if (computerPlaysAll())
        clearMarks()
    else
        clearMarksNotComputer()
    markLastMove()
    delayedCheckComputerMove.start()
}

function moveHint() {
    if (boardModel.isGameOver)
        return
    cancelGenMove()
    isMoveHintRunning = true
    gameDisplay.busyIndicatorRunning = true
    playerModel.startGenMoveAtLevel(boardModel, 1)
}

function newGame(verifyAbortGame)
{
    cancelGenMove()
    if (! boardModel.isBoardEmpty &&  ! boardModel.isGameOver &&
            verifyAbortGame) {
        showMessageDialog(qsTr("New game?"),
                          function() { newGame(false) })
        return
    }
    gameDisplay.pickedPiece = null
    gameDisplay.clearMessage()
    clearMarks()
    hideComputerColorDialog()
    gameDisplay.transitionsEnabled = false
    boardModel.newGame()
    gameDisplay.showToPlay()
    gameDisplay.transitionsEnabled = true
    initComputerColors()
}

function play(pieceModel, gameCoord) {
    cancelGenMove()
    var wasComputerToPlay = isComputerToPlay()
    boardModel.play(pieceModel, gameCoord)
    clearMarks()
    markLastMove()
    // If a move was entered but the computer plays this color (e.g. after
    // undo move) we don't automatically continue playing.
    if (! wasComputerToPlay)
        delayedCheckComputerMove.start()
}

function showComputerColorDialog() {
    if (computerColorDialogLoader.status == Loader.Null)
        computerColorDialogLoader.sourceComponent =
                computerColorDialogComponent
    var dialog = computerColorDialogLoader.item
    dialog.computerPlays0 = computerPlays0
    dialog.computerPlays1 = computerPlays1
    dialog.computerPlays2 = computerPlays2
    dialog.computerPlays3 = computerPlays3
    dialog.visible = true
}

function showGameOver() {
    if (! boardModel.isGameOver)
        return
    var msg, points0, points1, points2, points3
    switch (boardModel.gameVariant) {
    case "duo":
    case "junior":
        points0 = boardModel.points0
        points1 = boardModel.points1
        if (points0 > points1)
            msg = qsTr("Blue wins.")
        else if (points0 < points1)
            msg = qsTr("Green wins.")
        else
            msg = qsTr("Game ends in a tie.")
        break
    case "classic_2":
    case "trigon_2":
        points0 = boardModel.points0 + boardModel.points2
        points1 = boardModel.points1 + boardModel.points3
        if (points0 > points1)
            msg = qsTr("Blue/Red wins.")
        else if (points0 < points1)
            msg = qsTr("Yellow/Green wins.")
        else
            msg = qsTr("Game ends in a tie.")
        break
    case "classic_3":
    case "trigon_3":
        points0 = boardModel.points0
        points1 = boardModel.points1
        points2 = boardModel.points2
        var maxPoints = Math.max(points0, points1, points2)
        var nuWinners = 0
        if (points0 === maxPoints) ++nuWinners
        if (points1 === maxPoints) ++nuWinners
        if (points2 === maxPoints) ++nuWinners
        if (nuWinners > 1)
            msg = qsTr("Game ends in a tie.")
        else if (points0 === maxPoints)
            msg = qsTr("Blue wins.")
        else if (points1 === maxPoints)
            msg = qsTr("Yellow wins.")
        else if (points2 === maxPoints)
            msg = qsTr("Red wins.")
        break
    default:
        points0 = boardModel.points0
        points1 = boardModel.points1
        points2 = boardModel.points2
        points3 = boardModel.points3
        maxPoints = Math.max(points0, points1, points2, points3)
        nuWinners = 0
        if (points0 === maxPoints) ++nuWinners
        if (points1 === maxPoints) ++nuWinners
        if (points2 === maxPoints) ++nuWinners
        if (points3 === maxPoints) ++nuWinners
        if (nuWinners > 1)
            msg = qsTr("Game ends in a tie.")
        else if (points0 === maxPoints)
            msg = qsTr("Blue wins.")
        else if (points1 === maxPoints)
            msg = qsTr("Yellow wins.")
        else if (points2 === maxPoints)
            msg = qsTr("Red wins.")
        else if (points3 === maxPoints)
            msg = qsTr("Green wins.")
    }
    gameDisplay.showMessage(msg)
}

function showErrorDialog(text, acceptedFunc) {
    if (errorDialogLoader.status == Loader.Null)
        errorDialogLoader.sourceComponent = errorDialogComponent
    var dialog = errorDialogLoader.item
    dialog.text = text
    dialog.accepted.connect(acceptedFunc)
    dialog.visible = true
}

function showMessageDialog(text, acceptedFunc) {
    if (messageDialogLoader.status == Loader.Null)
        messageDialogLoader.sourceComponent = messageDialogComponent
    var dialog = messageDialogLoader.item
    dialog.text = text
    dialog.accepted.connect(acceptedFunc)
    dialog.visible = true
}

function undo() {
    if (! boardModel.canUndo)
        return
    cancelGenMove()
    gameDisplay.clearMessage()
    // Immediately show the last color to move, otherwise the piece movement
    // animation will target a part of the piece selector that is currently not
    // visible
    gameDisplay.showPiecesImmediately(boardModel.getLastMoveColor())
    boardModel.undo()
    clearMarks()
    markLastMove()
    gameDisplay.pickedPiece = null
}

function quit() {
    cancelGenMove()
    autosave()
}
