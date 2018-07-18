function about() {
    var url = "https://pentobi.sourceforge.io"
    // Don't use &copy; or &ndash;, this is currently not rendered in the label
    // of the message box (last tested with Qt 5.8-rc)
    showInfo("<h2>" + qsTr("Pentobi") + "</h2><br/>" +
             qsTr("Version %1").arg(Qt.application.version) + "<br/><br/>" +
             qsTr("Computer opponent for the board game Blokus.") + "<br/>" +
             qsTr("Copyright 2011-%1 Markus&nbsp;Enzenberger").arg(2018) +
             "<br><a href=\"" + url + "\">" + url + "</a></p>")
}

function analyzeGame(nuSimulations) {
    if (! gameModel.isMainVar) {
        showInfo(qsTr("Game analysis is only possible in main variation."))
        return
    }
    gameDisplay.showAnalyzeGame()
    Logic.cancelRunning()
    analyzeGameModel.start(gameModel, playerModel, nuSimulations)
}

function autoSaveAndQuit() {
    if (gameModel.checkAutosaveModifiedOutside()) {
        if (! gameModel.isModified || gameModel.isGameEmpty)
            return true
        showQuestion(qsTr("Autosaved game was changed by another instance of Pentobi. Overwrite?"),
                     autoSaveNoVerifyAndQuit)
        return false
    }
    autoSaveNoVerifyAndQuit()
    return true
}

function autoSaveNoVerify() {
    wasGenMoveRunning =
            (playerModel.isGenMoveRunning
             || delayedCheckComputerMove.running)
            && ! isPlaySingleMoveRunning
    gameModel.autoSave()
    settings.computerPlays0 = rootWindow.computerPlays0
    settings.computerPlays1 = rootWindow.computerPlays1
    settings.computerPlays2 = rootWindow.computerPlays2
    settings.computerPlays3 = rootWindow.computerPlays3
    settings.isRated = rootWindow.isRated
    settings.wasGenMoveRunning = rootWindow.wasGenMoveRunning
    analyzeGameModel.autoSave(gameModel)
    Logic.cancelRunning()
}

function autoSaveNoVerifyAndQuit() {
    autoSaveNoVerify()
    Qt.quit()
}

function cancelRunning() {
    if (analyzeGameModel.isRunning) {
        analyzeGameModel.cancel()
        showTemporaryMessage(qsTr("Game analysis aborted."))
    }
    if (playerModel.isGenMoveRunning) {
        playerModel.cancelGenMove()
        showTemporaryMessage(qsTr("Computer move aborted."))
    }
    delayedCheckComputerMove.stop()
}

function changeGameVariant(gameVariant) {
    if (gameModel.gameVariant === gameVariant)
        return
    verify(function() { changeGameVariantNoVerify(gameVariant) })
}

function changeGameVariantNoVerify(gameVariant) {
    cancelRunning()
    lengthyCommand.run(function() {
        gameDisplay.destroyPieces()
        gameModel.changeGameVariant(gameVariant)
        gameDisplay.createPieces()
        gameDisplay.showToPlay()
        gameDisplay.setupMode = false
        isRated = false
        analyzeGameModel.clear()
        gameDisplay.showPieces()
        initComputerColors()
    })
}

function checkComputerMove() {
    if (gameModel.isGameOver) {
        var msg = gameModel.getResultMessage()
        if (isRated) {
            var oldRating = Math.round(ratingModel.rating)
            ratingModel.addResult(gameModel, playerModel.level)
            var newRating = Math.round(ratingModel.rating)
            msg += "\n"
            if (newRating > oldRating)
                msg += qsTr("Your rating has increased from %1 to %2.").arg(oldRating).arg(newRating)
            else if (newRating < oldRating)
                msg += qsTr("Your rating has decreased from %1 to %2.").arg(oldRating).arg(newRating)
            else
                msg += qsTr("Your rating stays at %1.").arg(newRating)
            isRated = false
        }
        showInfo(msg)
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
    genMove()
}

function clearRating() {
    showQuestion(qsTr("Delete all rating information for the current game variant?"),
                 clearRatingNoVerify)
}

function clearRatingNoVerify() {
    ratingModel.clearRating()
    showTemporaryMessage(qsTr("Rating information deleted."))
}

/** If the computer already plays the current color to play, start generating
    a move; if he doesn't, make him play the current color (and only the
    current color). */
function computerPlay() {
    if (playerModel.isGenMoveRunning)
        return
    if (! isComputerToPlay()) {
        setComputerNone()
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
            switch (gameModel.toPlay) {
            case 0:
                computerPlays0 = true
                if (isMultiColor()) computerPlays2 = true
                break
            case 1:
                computerPlays1 = true
                if (isMultiColor()) computerPlays3 = true
                break
            case 2:
                computerPlays2 = true
                if (isMultiColor()) computerPlays0 = true
                break
            case 3:
                computerPlays3 = true
                if (isMultiColor()) computerPlays1 = true
                break
            }
        }
        initComputerColorsOnNewGame = true
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

function computerPlaysAny() {
    if (computerPlays0) return true
    if (computerPlays1) return true
    if (gameModel.nuColors > 2 && computerPlays2) return true
    if (gameModel.nuColors > 3 && computerPlays3) return true
    return false
}

function createTheme(themeName) {
    var source = "themes/" + themeName + "/Theme.qml"
    var component = Qt.createComponent(source)
    if (component.status !== Component.Ready) {
        // In case the settings contained an invalid theme name
        var source = "themes/light/Theme.qml"
        var component = Qt.createComponent(source)
    }
    return component.createObject(rootWindow)
}

function deleteAllVar() {
    showQuestion(qsTr("Delete all variations?"), deleteAllVarNoVerify)
}

function deleteAllVarNoVerify() {
    gameModel.deleteAllVar()
    showTemporaryMessage(qsTr("Variations deleted."))
}

function exportAsciiArt(fileUrl) {
    if (! gameModel.saveAsciiArt(getFileFromUrl(fileUrl)))
        showInfo(qsTr("Save failed.") + "\n" + gameModel.lastInputOutputError)
    else
        showTemporaryMessage(qsTr("File saved."))
}

function exportImage(fileUrl) {
    var board = gameDisplay.getBoard()
    var size = Qt.size(exportImageWidth, exportImageWidth * board.height / board.width)
    if (! board.grabToImage(function(result) {
        var file = getFileFromUrl(fileUrl)
        if (! result.saveToFile(file))
            showInfo(qsTr("Saving image failed."))
        else {
            gameModel.androidScanFile(file)
            showTemporaryMessage(qsTr("Image saved."))
        }
    }, size))
        showInfo(qsTr("Creating image failed."))
}

function findMove() {
    gameDisplay.showMove(gameModel.findMove())
}

function findNextComment() {
    if (gameModel.findNextComment()) {
        gameDisplay.showComment()
        return
    }
    if (gameModel.canGoBackward)
        // Current is not root
        showQuestion(qsTr("End of tree was reached. Continue search from start of the tree?"),
                     findNextCommentContinueFromRoot)
    else
        showInfo(qsTr("No comment found"))

}

function findNextCommentContinueFromRoot() {
    if (gameModel.findNextCommentContinueFromRoot()) {
        gameDisplay.showComment()
        return
    }
    showInfo(qsTr("No comment found"))
}

function genMove() {
    Logic.cancelRunning()
    gameDisplay.pickedPiece = null
    gameDisplay.showToPlay()
    playerModel.startGenMove(gameModel)
}

function getFileFromUrl(fileUrl) {
    var file = fileUrl.toString()
    file = file.replace(/^(file:\/{3})/,"/")
    return decodeURIComponent(file)
}

function getFileInfo(file, isModified) {
    if (isModified) return qsTr("%1 (modified)").arg(file)
    return file
}

function getFileLabel(file, isModified) {
    if (file === "")
        return ""
    var pos = Math.max(file.lastIndexOf("/"), file.lastIndexOf("\\"))
    return (isModified ? "*" : "") + file.substring(pos + 1)
}

function help() {
    if (helpWindow.status === Loader.Null)
        helpWindow.sourceComponent = helpWindow.component
    helpWindow.item.show()
    helpWindow.item.init()
}

function init() {
    gameModel.loadAutoSave()
    rootWindow.computerPlays0 = settings.computerPlays0
    rootWindow.computerPlays1 = settings.computerPlays1
    rootWindow.computerPlays2 = settings.computerPlays2
    rootWindow.computerPlays3 = settings.computerPlays3
    rootWindow.isRated = settings.isRated
    rootWindow.wasGenMoveRunning = settings.wasGenMoveRunning
    gameDisplay.createPieces()
    if (gameModel.checkFileDeletedOutside())
    {
        showInfo(qsTr("File was deleted by another application."))
        gameModel.isModified = true
    }
    if (gameModel.checkFileModifiedOutside())
    {
        showQuestion(qsTr("File has been modified by another application. Reload?"), reloadFile)
        return
    }
    analyzeGameModel.loadAutoSave(gameModel)
    // initialFile is a context property set from command line argument
    if (initialFile) {
        if (gameModel.isModified)
            rootWindow.show()
        verify(function() { openFileBlocking(initialFile) })
    }
    if (wasGenMoveRunning || isRated)
        checkComputerMove()
}

function initComputerColors() {
    if (! initComputerColorsOnNewGame)
        return
    // Default setting is that the computer plays all colors but the first
    computerPlays0 = false
    computerPlays1 = true
    computerPlays2 = true
    computerPlays3 = true
    if (isMultiColor())
        computerPlays2 = false
}

function isComputerToPlay() {
    if (gameModel.gameVariant == "classic_3" && gameModel.toPlay == 3)
        return computerPlays(gameModel.altPlayer)
    return computerPlays(gameModel.toPlay)
}

function isMultiColor() {
    return gameModel.nuColors == 4 && gameModel.nuPlayers == 2

}

function keepOnlyPosition() {
    showQuestion(qsTr("Keep only position?"), keepOnlyPositionNoVerify)
}

function keepOnlyPositionNoVerify() {
    gameModel.keepOnlyPosition()
    showTemporaryMessage(qsTr("Kept only position."))
}

function keepOnlySubtree() {
    showQuestion(qsTr("Keep only subtree?"), keepOnlySubtreeNoVerify)
}

function keepOnlySubtreeNoVerify() {
    gameModel.keepOnlySubtree()
    showTemporaryMessage(qsTr("Kept only subtree."))
}

function moveDownVar() {
    gameModel.moveDownVar()
    showVariationInfo()
}

function moveGenerated(move) {
    gameModel.playMove(move)
    if (isPlaySingleMoveRunning)
        isPlaySingleMoveRunning = false
    else
        delayedCheckComputerMove.restart()
}

function moveUpVar() {
    gameModel.moveUpVar()
    showVariationInfo()
}

function newGame()
{
    verify(newGameNoVerify)
}

function newGameNoVerify()
{
    gameModel.newGame()
    gameDisplay.setupMode = false
    gameDisplay.showToPlay()
    gameDisplay.showPieces()
    isRated = false
    analyzeGameModel.clear()
    initComputerColors()
}

function nextPiece() {
    var currentPickedPiece = null
    if (gameDisplay.pickedPiece)
        currentPickedPiece = gameDisplay.pickedPiece.pieceModel
    var pieceModel = gameModel.nextPiece(currentPickedPiece)
    if (pieceModel)
        gameDisplay.pickPieceAtBoard(gameDisplay.findPiece(pieceModel))
}

function open() {
    verify(openNoVerify)
}

function openNoVerify() {
    openDialog.open()
}

function openRatedGame(byteArray) {
    verify(function() { openRatedGameNoVerify(byteArray) })
}

function openRatedGameNoVerify(byteArray) {
    lengthyCommand.run(function() {
        var oldGameVariant = gameModel.gameVariant
        var oldEnableAnimations = gameDisplay.enableAnimations
        gameDisplay.enableAnimations = false
        if (! gameModel.openByteArray(byteArray))
            showInfo(qsTr("Open failed.") + "\n" + gameModel.lastInputOutputError)
        setComputerNone()
        if (gameModel.gameVariant != oldGameVariant) {
            gameDisplay.destroyPieces()
            gameDisplay.createPieces()
        }
        gameDisplay.showToPlay()
        gameDisplay.setupMode = false
        isRated = false
        analyzeGameModel.clear()
        gameDisplay.showToPlay()
        gameDisplay.enableAnimations = oldEnableAnimations
    })
}

function openFile(file) {
    lengthyCommand.run(function() { openFileBlocking(file) })
}

function openFileBlocking(file) {
    var oldGameVariant = gameModel.gameVariant
    var oldEnableAnimations = gameDisplay.enableAnimations
    gameDisplay.enableAnimations = false
    if (! gameModel.openFile(file))
        showInfo(qsTr("Open failed.") + "\n" + gameModel.lastInputOutputError)
    else
        setComputerNone()
    if (gameModel.gameVariant != oldGameVariant) {
        gameDisplay.destroyPieces()
        gameDisplay.createPieces()
    }
    gameDisplay.showToPlay()
    gameDisplay.enableAnimations = oldEnableAnimations
    gameDisplay.setupMode = false
    isRated = false
    analyzeGameModel.clear()
    if (gameModel.comment.length > 0)
        gameDisplay.showComment()
    else
        gameDisplay.showPieces()
}

function openFileUrl() {
    openFile(getFileFromUrl(openDialog.item.fileUrl))
}

function openClipboard()
{
    verify(openClipboardNoVerify)
}

function openClipboardNoVerify() {
    lengthyCommand.run(function() {
        var oldGameVariant = gameModel.gameVariant
        var oldEnableAnimations = gameDisplay.enableAnimations
        gameDisplay.enableAnimations = false
        if (! gameModel.openClipboard())
            showInfo(qsTr("Open failed.") + "\n" + gameModel.lastInputOutputError)
        else
            setComputerNone()
        if (gameModel.gameVariant != oldGameVariant) {
            gameDisplay.destroyPieces()
            gameDisplay.createPieces()
        }
        gameDisplay.showToPlay()
        gameDisplay.enableAnimations = oldEnableAnimations
        gameDisplay.setupMode = false
        isRated = false
        analyzeGameModel.clear()
    })
}

function openGameInfoDialog() {
    gameInfoDialog.open()
    var dialog = gameInfoDialog.item
    dialog.playerName0 = gameModel.playerName0
    dialog.playerName1 = gameModel.playerName1
    dialog.playerName2 = gameModel.playerName2
    dialog.playerName3 = gameModel.playerName3
    dialog.date = gameModel.date
    dialog.time = gameModel.time
    dialog.event = gameModel.event
    dialog.round = gameModel.round
}

function openRecentFile(file) {
    verify(function() { openFile(file) })
}

function pickNamedPiece(name) {
    var currentPickedPiece = null
    if (gameDisplay.pickedPiece)
        currentPickedPiece = gameDisplay.pickedPiece.pieceModel
    var pieceModel = gameModel.pickNamedPiece(name, currentPickedPiece)
    if (pieceModel)
        gameDisplay.pickPieceAtBoard(gameDisplay.findPiece(pieceModel))
}

function play(pieceModel, gameCoord) {
    var wasComputerToPlay = isComputerToPlay()
    gameModel.playPiece(pieceModel, gameCoord)
    // We don't continue automatic play if the human played a move for a color
    // played by the computer.
    if (! wasComputerToPlay)
        delayedCheckComputerMove.restart()
}

function prevPiece() {
    var currentPickedPiece = null
    if (gameDisplay.pickedPiece)
        currentPickedPiece = gameDisplay.pickedPiece.pieceModel
    var pieceModel = gameModel.previousPiece(currentPickedPiece)
    if (pieceModel)
        gameDisplay.pickPieceAtBoard(gameDisplay.findPiece(pieceModel))
}

function ratedGame()
{
    verify(ratedGameCheckFirstGame)
}

function ratedGameCheckFirstGame() {
    if (ratingModel.numberGames === 0)
        initialRatingDialog.open()
    else
        ratedGameNoVerify()
}

function ratedGameNoVerify()
{
    var player = ratingModel.getNextHumanPlayer()
    var level = ratingModel.getNextLevel(playerModel.maxLevel)
    var gameVariant = gameModel.gameVariant
    var msg
    switch (player) {
    case 0:
        if (gameVariant === "duo")
            msg = qsTr("Start rated game with Purple against Pentobi level %1?").arg(level)
        else if (gameVariant === "junior")
            msg = qsTr("Start rated game with Green against Pentobi level %1?").arg(level)
        else if (isMultiColor())
            msg = qsTr("Start rated game with Blue/Red against Pentobi level %1?").arg(level)
        else
            msg = qsTr("Start rated game with Blue against Pentobi level %1?").arg(level)
        break
    case 1:
        if (gameVariant === "duo" || gameVariant === "junior")
            msg = qsTr("Start rated game with Orange against Pentobi level %1?").arg(level)
        else if (isMultiColor())
            msg = qsTr("Start rated game with Yellow/Green against Pentobi level %1?").arg(level)
        else if (gameModel.nuColors === 2)
            msg = qsTr("Start rated game with Green against Pentobi level %1?").arg(level)
        else
            msg = qsTr("Start rated game with Yellow against Pentobi level %1?").arg(level)
        break
    case 2:
        msg = qsTr("Start rated game with Red against Pentobi level %1?").arg(level)
        break
    case 3:
        msg = qsTr("Start rated game with Green against Pentobi level %1?").arg(level)
        break
    }
    showQuestion(msg, ratedGameStart)
}

function ratedGameStart() {
    var player = ratingModel.getNextHumanPlayer()
    computerPlays0 = (player !== 0)
    computerPlays1 = (player !== 1)
    computerPlays2 = (player !== 2)
    computerPlays3 = (player !== 3)
    if (isMultiColor()) {
        computerPlays2 = computerPlays0
        computerPlays3 = computerPlays1
    }
    playerModel.level = ratingModel.getNextLevel(playerModel.maxLevel)
    gameModel.newGame()
    gameDisplay.setupMode = false
    gameDisplay.showToPlay()
    gameDisplay.showPieces()
    isRated = true
    analyzeGameModel.clear()
    checkComputerMove()
}

function reloadFile() {
    openFile(gameModel.file)
}

function save() {
    if (gameModel.checkFileModifiedOutside())
        showQuestion(qsTr("File has been modified by another application. Save anyway?"),
                     saveCurrentFile)
    else
        saveCurrentFile()
}

function saveAs() {
    var dialog = saveDialog.get()
    dialog.name = gameModel.suggestFileName(rootWindow.folder)
    dialog.open()
}

function saveCurrentFile() {
    saveFile(gameModel.file)
}

function saveFile(file) {
    if (! gameModel.save(file))
        showInfo(qsTr("Save failed.") + "\n" + gameModel.lastInputOutputError)
    else
        showTemporaryMessage(qsTr("File saved."))
}

function setComputerNone() {
    computerPlays0 = false
    computerPlays1 = false
    computerPlays2 = false
    computerPlays3 = false
}

function showComputerDialog() {
    var dialog = computerDialogLoader.get()
    dialog.computerPlays0 = computerPlays0
    dialog.computerPlays1 = computerPlays1
    dialog.computerPlays2 = computerPlays2
    dialog.computerPlays3 = computerPlays3
    dialog.level = playerModel.level
    dialog.open()
}

function showInfo(text) {
    var dialog = infoMessage.get()
    dialog.text = text
    dialog.open()
}

function showInfoWithCallback(text, acceptedFunc) {
    infoMessage.get().openWithCallback(text, acceptedFunc)
}

function showQuestion(text, acceptedFunc) {
    questionMessage.get().openWithCallback(text, acceptedFunc)
}

function showTemporaryMessage(text) {
    gameDisplay.showTemporaryMessage(text)
}

function showVariationInfo() {
    showTemporaryMessage(qsTr("Variation is now %1.").arg(gameModel.getVariationInfo()))
}

function truncate() {
    showQuestion(qsTr("Truncate this subtree?"), gameModel.truncate)
}

function truncateChildren() {
    showQuestion(qsTr("Truncate children?"), truncateChildrenNoVerify)
}

function truncateChildrenNoVerify() {
    gameModel.truncateChildren()
    showTemporaryMessage(qsTr("Children truncated."))
}

function undo() {
    gameModel.undo()
}

function verify(callback)
{
    if (gameModel.isModified) {
        showQuestion(qsTr("Discard game?"), callback)
        return
    }
    callback()
}
