function autosave() {
    if (gameModel.isGameOver)
        gameModel.clearAutoSave()
    else
        gameModel.autoSave()
}

function cancelGenMove() {
    playerModel.cancelGenMove()
    gameDisplay.busyIndicatorRunning = false
}

function changeGameVariant(gameVariant, verifyAbortGame) {
    if (gameModel.gameVariant === gameVariant)
        return
    cancelGenMove()
    if (! gameModel.isBoardEmpty && ! gameModel.isGameOver &&
            verifyAbortGame) {
        showQuestion(qsTr("New game?"),
                     function() { changeGameVariant(gameVariant, false) })
        return
    }
    callDelayTimer.call(function() {
        initGameVariant(gameVariant)
        gameDisplay.transitionsEnabled = false
        initComputerColors()
        gameDisplay.showToPlay()
    })
}

function checkComputerMove() {
    if (gameModel.isGameOver) {
        showGameOver()
        return
    }
    if (! isComputerToPlay())
        return
    switch (gameModel.toPlay) {
    case 0: if (! gameModel.hasMoves0) return; break
    case 1: if (! gameModel.hasMoves1) return; break
    case 2: if (! gameModel.hasMoves2) return; break
    case 3: if (! gameModel.hasMoves3) return; break
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
        var variant = gameModel.gameVariant
        if (variant == "classic_3" && gameModel.toPlay == 3) {
            switch (gameModel.altPlayer) {
            case 0: computerPlays0 = true; break
            case 1: computerPlays1 = true; break
            case 2: computerPlays2 = true; break
            }
        }
        else
        {
            var isMultiColor =
                    (variant == "classic_2" || variant == "trigon_2")
            switch (gameModel.toPlay) {
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
    switch (gameModel.gameVariant) {
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
    if (component.status !== Component.Ready)
        throw "Could not load " + source
    return component.createObject(root)
}

function genMove() {
    cancelGenMove()
    gameDisplay.pickedPiece = null
    gameDisplay.busyIndicatorRunning = true
    gameDisplay.showToPlay()
    isMoveHintRunning = false
    playerModel.startGenMove(gameModel)
}

function hideComputerColorDialog()
{
    if (computerColorDialogLoader.status == Loader.Ready)
        computerColorDialogLoader.item.visible = false
}

function init() {
    callDelayTimer.call(function() {
        gameDisplay.createPieces()
        if (! gameModel.loadAutoSave())
            initComputerColors()
        else {
            clearMarks()
            markLastMove()
            if (! computerPlaysAll())
                checkComputerMove()
        }
    })
}

function initComputerColors() {
    // Default setting is that the computer plays all colors but the first
    computerPlays0 = false
    computerPlays1 = true
    computerPlays2 = true
    computerPlays3 = true
    if (gameModel.gameVariant == "classic_2"
            || gameModel.gameVariant == "trigon_2")
        computerPlays2 = false
}

function initGameVariant(gameVariant) {
    cancelGenMove()
    hideComputerColorDialog()
    clearMarks()
    gameDisplay.destroyPieces()
    gameModel.initGameVariant(gameVariant)
    gameDisplay.createPieces()
}

function isComputerToPlay() {
    if (gameModel.gameVariant == "classic_3" && gameModel.toPlay == 3)
        return computerPlays(gameModel.altPlayer)
    return computerPlays(gameModel.toPlay)
}

function markLastMove() {
    if (! root.markLastMove)
        return
    var color = gameModel.getLastMoveColor()
    var piece = gameDisplay.findPiece(gameModel.getLastMovePieceModel(), color)
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
    gameDisplay.transitionsEnabled = true
    gameModel.playMove(move)
    if (computerPlaysAll())
        clearMarks()
    else
        clearMarksNotComputer()
    markLastMove()
    delayedCheckComputerMove.start()
}

function moveHint() {
    if (gameModel.isGameOver)
        return
    cancelGenMove()
    isMoveHintRunning = true
    gameDisplay.busyIndicatorRunning = true
    playerModel.startGenMoveAtLevel(gameModel, 1)
}

function newGame(verifyAbortGame)
{
    cancelGenMove()
    if (! gameModel.isBoardEmpty &&  ! gameModel.isGameOver &&
            verifyAbortGame) {
        showQuestion(qsTr("New game?"), function() { newGame(false) })
        return
    }
    gameDisplay.pickedPiece = null
    clearMarks()
    hideComputerColorDialog()
    gameDisplay.transitionsEnabled = false
    gameModel.newGame()
    gameDisplay.showToPlay()
    initComputerColors()
}

function play(pieceModel, gameCoord) {
    cancelGenMove()
    var wasComputerToPlay = isComputerToPlay()
    gameDisplay.transitionsEnabled = true
    gameModel.play(pieceModel, gameCoord)
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
    if (! gameModel.isGameOver)
        return
    var msg, points0, points1, points2, points3
    switch (gameModel.gameVariant) {
    case "duo":
    case "junior":
        points0 = gameModel.points0
        points1 = gameModel.points1
        if (points0 > points1)
            msg = qsTr("Blue wins.")
        else if (points0 < points1)
            msg = qsTr("Green wins.")
        else
            msg = qsTr("Game ends in a tie.")
        break
    case "classic_2":
    case "trigon_2":
        points0 = gameModel.points0 + gameModel.points2
        points1 = gameModel.points1 + gameModel.points3
        if (points0 > points1)
            msg = qsTr("Blue/Red wins.")
        else if (points0 < points1)
            msg = qsTr("Yellow/Green wins.")
        else
            msg = qsTr("Game ends in a tie.")
        break
    case "classic_3":
    case "trigon_3":
        points0 = gameModel.points0
        points1 = gameModel.points1
        points2 = gameModel.points2
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
        points0 = gameModel.points0
        points1 = gameModel.points1
        points2 = gameModel.points2
        points3 = gameModel.points3
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
    showInfo(msg)
}

function showInfo(text) {
    if (infoMessageLoader.status === Loader.Null)
        infoMessageLoader.sourceComponent = infoMessageComponent
    var dialog = infoMessageLoader.item
    dialog.text = text
    dialog.visible = true
}

function showQuestion(text, acceptedFunc) {
    if (questionMessageLoader.status === Loader.Null)
        questionMessageLoader.sourceComponent = questionMessageComponent
    var dialog = questionMessageLoader.item
    dialog.text = text
    dialog.accepted.connect(acceptedFunc)
    dialog.visible = true
}

function undo() {
    if (! gameModel.canUndo)
        return
    cancelGenMove()
    // Immediately show the last color to move, otherwise the piece movement
    // animation will target a part of the piece selector that is currently not
    // visible
    gameDisplay.showPiecesImmediately(boardModel.getLastMoveColor())
    gameDisplay.transitionsEnabled = true
    gameModel.undo()
    clearMarks()
    markLastMove()
    gameDisplay.pickedPiece = null
}

function quit() {
    cancelGenMove()
    autosave()
}
