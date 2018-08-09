//-----------------------------------------------------------------------------
/** @file pentobi/qml/Main.js
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

function analyzeGame(nuSimulations) {
    if (! gameModel.isMainVar) {
        showInfo(qsTr("Game analysis is only possible in main variation."))
        return
    }
    gameDisplay.startAnalysis();
    cancelRunning()
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
    settings.computerPlays0 = computerPlays0
    settings.computerPlays1 = computerPlays1
    settings.computerPlays2 = computerPlays2
    settings.computerPlays3 = computerPlays3
    settings.isRated = isRated
    settings.wasGenMoveRunning = wasGenMoveRunning
    analyzeGameModel.autoSave(gameModel)
}

function autoSaveNoVerifyAndQuit() {
    autoSaveNoVerify()
    Qt.quit()
}

function cancelRunning(showMessage) {
    if (analyzeGameModel.isRunning) {
        analyzeGameModel.cancel()
        if (showMessage)
            showTemporaryMessage(qsTr("Game analysis aborted."))
    }
    if (playerModel.isGenMoveRunning) {
        playerModel.cancelGenMove()
        if (showMessage)
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

function checkStoragePermission() {
    if (! androidUtils.checkPermission("android.permission.WRITE_EXTERNAL_STORAGE")) {
        showInfo(qsTr("No permission to access storage"))
        return false
    }
    return true
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
        console.warn(component.errorString())
        source = "themes/light/Theme.qml"
        component = Qt.createComponent(source)
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
    if (! checkStoragePermission())
        return
    if (! gameModel.saveAsciiArt(getFileFromUrl(fileUrl)))
        showInfo(qsTr("Save failed.") + "\n" + gameModel.lastInputOutputError)
    else {
        androidUtils.scanFile(file)
        showTemporaryMessage(qsTr("File saved."))
    }
}

function exportImage(fileUrl) {
    if (! checkStoragePermission())
        return
    var board = gameDisplay.getBoard()
    var size = Qt.size(exportImageWidth, exportImageWidth * board.height / board.width)
    if (! board.grabImageTarget.grabToImage(function(result) {
        var file = getFileFromUrl(fileUrl)
        if (! result.saveToFile(file))
            showInfo(qsTr("Saving image failed or unsupported image format."))
        else {
            androidUtils.scanFile(file)
            showTemporaryMessage(qsTr("Image saved."))
        }
    }, size))
        showInfo(qsTr("Creating image failed."))
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
    cancelRunning()
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
    if (! helpWindow.item)
        helpWindow.sourceComponent = helpWindowComponent
    helpWindow.item.show()
    helpWindow.item.init()
}

function init() {
    gameModel.loadAutoSave()
    computerPlays0 = settings.computerPlays0
    computerPlays1 = settings.computerPlays1
    computerPlays2 = settings.computerPlays2
    computerPlays3 = settings.computerPlays3
    isRated = settings.isRated
    wasGenMoveRunning = settings.wasGenMoveRunning
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
            show()
        verify(function() { openFileBlocking(initialFile) })
    }
    if (isRated) {
        showInfo(qsTr("Continuing rated game."))
        checkComputerMove()
        return
    }
    if (wasGenMoveRunning)
        showQuestion(qsTr("Continue computer move?"), checkComputerMove)
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

function leaveFullscreen() {
    rootWindow.visibility = Window.AutomaticVisibility
    // Leaving fullscreen doesn't always work on Android but leaves
    // a white area between the window top and the Android status
    // bar (last tested with Qt 5.11.1)
    rootWindow.x = 0
    rootWindow.y = 0
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
    if (! checkStoragePermission())
        return
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
        if (gameModel.gameVariant != oldGameVariant)
            gameDisplay.createPieces()
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
    if (gameModel.gameVariant != oldGameVariant)
        gameDisplay.createPieces()
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
        if (gameModel.gameVariant != oldGameVariant)
            gameDisplay.createPieces()
        gameDisplay.showToPlay()
        gameDisplay.enableAnimations = oldEnableAnimations
        gameDisplay.setupMode = false
        isRated = false
        analyzeGameModel.clear()
    })
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

function rating() {
    if (ratingModel.numberGames === 0) {
        showInfo(qsTr("You have not yet played rated games in this game variant."))
        return
    }
    // Never reuse RatingDialog
    // See comment in Main.qml at ratingModel.onHistoryChanged
    ratingDialog.sourceComponent = null
    ratingDialog.open()
}

function reloadFile() {
    openFile(gameModel.file)
}

function save() {
    if (! checkStoragePermission())
        return
    if (gameModel.checkFileModifiedOutside())
        showQuestion(qsTr("File has been modified by another application. Save anyway?"),
                     saveCurrentFile)
    else
        saveCurrentFile()
}

function saveAs() {
    if (! checkStoragePermission())
        return
    var dialog = saveDialog.get()
    dialog.name = gameModel.suggestGameFileName(folder)
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

function showFatal(text) {
    var dialog = fatalMessage.get()
    dialog.text = text
    dialog.open()
}

function showInfo(text) {
    var dialog = infoMessage.get()
    dialog.text = text
    dialog.open()
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
    else if (gameModel.checkFileDeletedOutside()) {
        showQuestion(qsTr("File was deleted by another application. Discard game?"),
                     callback)
        return
    }
    callback()
}
