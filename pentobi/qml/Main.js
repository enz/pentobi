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
    gameView.startAnalysis()
    cancelRunning()
    analyzeGameModel.start(gameModel, playerModel, nuSimulations)
}

function autoSaveNoVerify() {
    gameModel.autoSave()
    syncSettings.setValueBool("computerPlays0", computerPlays0)
    syncSettings.setValueBool("computerPlays1", computerPlays1)
    syncSettings.setValueBool("computerPlays2", computerPlays2)
    syncSettings.setValueBool("computerPlays3", computerPlays3)
    syncSettings.setValueBool("isRated", isRated)
    syncSettings.setValueBool("initComputerColorsOnNewGame", initComputerColorsOnNewGame)
    syncSettings.setValueInt("level", playerModel.level)
    syncSettings.sync()
    analyzeGameModel.autoSave(gameModel)
    if (visibility === Window.Windowed) {
        settings.x = x
        settings.y = y
        settings.width = width
        settings.height = height
    }
}

function autoSaveNoVerifyAndQuit() {
    autoSaveNoVerify()
    if (isAndroid)
        androidUtils.exit()
    else
        Qt.quit()
}

function cancelRunning(showMessage) {
    if (analyzeGameModel.isRunning) {
        analyzeGameModel.cancel()
        if (showMessage)
            showTemporaryMessage(qsTr("Game analysis aborted"))
    }
    if (playerModel.isGenMoveRunning) {
        playerModel.cancelGenMove()
        if (showMessage)
            showTemporaryMessage(qsTr("Computer move aborted"))
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
        // Destroy pieces before changing game variant to avoid flickering
        // in PieceSelectorMobile if toPlay != 0
        gameView.destroyPieces()
        gameModel.changeGameVariant(gameVariant)
        gameView.createPieces()
        gameView.showToPlay()
        gameView.setupMode = false
        isRated = false
        analyzeGameModel.clear()
        gameView.showPieces()
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
            recentFiles.add(gameModel.file, "")
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
    showTemporaryMessage(qsTr("Rating information deleted"))
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
        if (variant === "classic_3" && gameModel.toPlay === 3) {
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
    showTemporaryMessage(qsTr("Variations deleted"))
}

function expirePersistedUriPermissions() {
    var uris = androidUtils.getPersistedUriPermissions()
    var len = uris.length
    for (var i = 0; i < len; ++i)
        if (! recentFilesContains(uris[i]) && uris[i] !== initialFile
                && uris[i] !== gameModel.file)
            androidUtils.releasePersistableUriPermission(uris[i])
}

function exportAsciiArt(fileUrl) {
    var file = isAndroid ? fileUrl : getFileFromUrl(fileUrl)
    if (! gameModel.saveAsciiArt(file))
        showInfo(qsTr("Save failed.") + "\n" + gameModel.getError())
    else
        showTemporaryMessage(qsTr("File saved"))
}

function exportImage(fileUrl) {
    var board = gameView.getBoard()
    var size = Qt.size(exportImageWidth, exportImageWidth * board.height / board.width)
    if (! board.grabImageTarget.grabToImage(function(result) {
        var ok
        if (isAndroid)
            ok = androidUtils.saveImage(fileUrl, result.image)
        else
            ok = result.saveToFile(getFileFromUrl(fileUrl))
        if (! ok)
            showInfo(qsTr("Saving image failed or unsupported image format"))
        else
            showTemporaryMessage(qsTr("Image saved"))
    }, size))
        showInfo(qsTr("Creating image failed"))
}

function findNextComment() {
    if (gameModel.findNextComment()) {
        gameView.showComment()
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
    if (gameModel.findNextCommentContinueFromRoot())
        gameView.showComment()
    else
        showInfo(qsTr("No comment found"))
}

function genMove() {
    cancelRunning()
    gameView.pickedPiece = null
    gameView.showToPlay()
    playerModel.startGenMove(gameModel)
}

function getFileFromUrl(fileUrl) {
    var file = fileUrl.toString()
    file = file.replace(/^(file:\/{3})/,"/")
    if (Qt.platform.os == "windows")
        file = file.replace(/^\/(\w:)/,"$1").replace(/\//g,"\\")
    return decodeURIComponent(file)
}

function getFileInfo(isRated, file, isModified) {
    if (isRated)
        //: Label for rated game. The argument is the game number.
        return qsTr("Rated Game %1").arg(ratingModel.numberGames + 1)
    if (isModified)
        //: Label for modified loaded game. The argument is the file name.
        return qsTr("%1 (modified)").arg(file)
    return file
}

function getGameLabel(setupMode, isRated, file, isModified, short) {
    if (setupMode)
        return short ?
                    //: Small-screen label for setup mode (short for
                    //: "Setup Mode").
                    qsTr("Setup")
                  : qsTr("Setup Mode")
    if (isRated)
        //: Label for ongoing rated game
        return qsTr("Rated")
    if (file === "")
        return ""
    var label
    var n = ratingModel.getGameNumberOfFile(file)
    if (n > 0)
        label = short ?
                    //: Small-screen label for finished rated game (short for
                    //: "Rated Game"). The argument is the game number.
                    qsTr("Rated %1").arg(n)
                  : qsTr("Rated Game %1").arg(n)
    else {
        if (isAndroid && displayName !== "")
            label = displayName
        else {
            var pos = Math.max(file.lastIndexOf("/"), file.lastIndexOf("\\"))
            label = file.substring(pos + 1)
        }
        label = label.replace("\.blksgf", "")
    }
    return (isModified ? "*" : "") + label
}

function getPlayerString(variant, player) {
    var isMultiColor = (variant === "classic_2" || variant === "trigon_2"
                        || variant === "nexos_2" || variant === "callisto_2_4"
                        || variant === "gembloq_2_4")
    switch (player) {
    case 0:
        if (isMultiColor)
            return qsTr("Blue/Red");
        else if (variant === "duo")
            return qsTr("Purple");
        else if (variant === "junior")
            return qsTr("Green");
        else
            return qsTr("Blue");
    case 1:
        if (isMultiColor)
            return qsTr("Yellow/Green");
        else if (variant === "duo" || variant === "junior")
            return qsTr("Orange");
        else if (variant === "callisto_2" || variant === "gembloq_2")
            return qsTr("Green");
        else
            return qsTr("Yellow");
    case 2:
        return qsTr("Red");
    default:
        return qsTr("Green");
    }
}

function getWindowTitle(file, isModified) {
    if (file === "")
        //: Window title if no file is loaded.
        return qsTr("Pentobi")
    var pos = Math.max(file.lastIndexOf("/"), file.lastIndexOf("\\"))
    var name = file.substring(pos + 1)
    if (isModified)
        name = "*" + name
    //: Window title if file is loaded. The argument is the file name
    //: prepended with a star if the file has been modified.
    return qsTr("%1 - Pentobi").arg(name)
}

function init() {
    if (gameModel.loadAutoSave()) {
        computerPlays0 =
                syncSettings.valueBool("computerPlays0", computerPlays0)
        computerPlays1 =
                syncSettings.valueBool("computerPlays1", computerPlays1)
        computerPlays2 =
                syncSettings.valueBool("computerPlays2", computerPlays2)
        computerPlays3 =
                syncSettings.valueBool("computerPlays3", computerPlays3)
        isRated = syncSettings.valueBool("isRated", isRated)
        initComputerColorsOnNewGame =
                syncSettings.valueBool("initComputerColorsOnNewGame",
                                       initComputerColorsOnNewGame)
        analyzeGameModel.loadAutoSave(gameModel)
    }
    if (isAndroid)
        expirePersistedUriPermissions(initialFile, gameModel.file)
    playerModel.level = syncSettings.valueInt("level", 1)
    if (isMultiColor()) {
        computerPlays2 = computerPlays0
        computerPlays3 = computerPlays1
    }
    gameView.createPieces()
    if (gameModel.checkFileModifiedOutside())
    {
        showWindow()
        showQuestion(qsTr("File has been modified by another application. Reload?"),
                     reloadFile)
        return
    }
    if (analyzeGameModel.elements.length > 0)
        gameView.analysisAutoloaded()
    // initialFile is a context property set from command line argument
    if (initialFile) {
        if (! gameModel.isModified) {
            openFileBlocking(initialFile, displayName )
            showWindow()
        }
        else {
            showWindow()
            verify(function() { openFileBlocking(initialFile, displayName ) })
        }
        return
    }
    showWindow()
    if (isRated) {
        // Game-related properties in settings could be inconsistent with
        // autosaved game, better initialize with info from ratingModel
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
        showInfo(qsTr("Continuing rated game"))
        checkComputerMove()
        return
    }
    if (isComputerToPlay() && ! gameModel.canGoForward
            && ! gameModel.isGameOver && ! initialFile)
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
    if (gameModel.gameVariant == "classic_3" && gameModel.toPlay === 3)
        return computerPlays(gameModel.altPlayer)
    return computerPlays(gameModel.toPlay)
}

function isMultiColor() {
    return gameModel.nuColors === 4 && gameModel.nuPlayers === 2
}

function keepOnlyPosition() {
    showQuestion(qsTr("Keep only position?"), keepOnlyPositionNoVerify)
}

function keepOnlyPositionNoVerify() {
    gameModel.keepOnlyPosition()
    showTemporaryMessage(qsTr("Kept only position"))
}

function keepOnlySubtree() {
    showQuestion(qsTr("Keep only subtree?"), keepOnlySubtreeNoVerify)
}

function keepOnlySubtreeNoVerify() {
    gameModel.keepOnlySubtree()
    showTemporaryMessage(qsTr("Kept only subtree"))
}

function moveDownVar() {
    gameModel.moveDownVar()
    showVariationInfo()
}

function moveGenerated(move) {
    if (move.isNull()) {
        showInfo(qsTr("Pentobi failed to generate a move."))
        isPlaySingleMoveRunning = false
        return
    }
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

function newGame() {
    verify(newGameNoVerify)
}

function newGameNoVerify() {
    gameModel.newGame()
    gameView.setupMode = false
    gameView.showToPlay()
    gameView.showPieces()
    isRated = false
    analyzeGameModel.clear()
    initComputerColors()
}

function nextPiece() {
    var currentPickedPiece = null
    if (gameView.pickedPiece)
        currentPickedPiece = gameView.pickedPiece.pieceModel
    var pieceModel = gameModel.nextPiece(currentPickedPiece)
    if (pieceModel)
        gameView.pickPieceAtBoard(gameView.findPiece(pieceModel))
}

function open() {
    verify(openNoVerify)
}

function openNoVerify() {
    if (isAndroid)
        androidUtils.openOpenDialog(gameModel.file)
    else
        openDialog.open()
}

function openFile(file, displayName) {
    lengthyCommand.run(function() { openFileBlocking(file, displayName) })
}

function openFileBlocking(file, displayName) {
    var oldGameVariant = gameModel.gameVariant
    var oldEnableAnimations = gameView.enableAnimations
    gameView.enableAnimations = false
    if (! gameModel.openFile(file))
        showInfo(qsTr("Open failed.") + "\n" + gameModel.getError())
    else {
        if (isAndroid)
            displayName = androidUtils.getDisplayName(file)
        rootWindow.displayName = displayName
        recentFiles.add(file, displayName)
        setComputerNone()
    }
    if (gameModel.gameVariant !== oldGameVariant)
        gameView.createPieces()
    gameView.showToPlay()
    gameView.enableAnimations = oldEnableAnimations
    gameView.setupMode = false
    isRated = false
    analyzeGameModel.clear()
    if (gameModel.comment.length > 0)
        gameView.showComment()
    else
        gameView.showPieces()
}

function openFileUrl(fileUrl) {
    openFile(getFileFromUrl(fileUrl), "")
}

function openClipboard() {
    verify(openClipboardNoVerify)
}

function openClipboardNoVerify() {
    lengthyCommand.run(function() {
        var oldGameVariant = gameModel.gameVariant
        var oldEnableAnimations = gameView.enableAnimations
        gameView.enableAnimations = false
        if (! gameModel.openClipboard())
            showInfo(qsTr("Open failed.") + "\n" + gameModel.getError())
        else
            setComputerNone()
        if (gameModel.gameVariant !== oldGameVariant)
            gameView.createPieces()
        gameView.showToPlay()
        gameView.enableAnimations = oldEnableAnimations
        gameView.setupMode = false
        isRated = false
        analyzeGameModel.clear()
    })
}

function openRecentFile(file, displayName) {
    verify(function() { openFile(file, displayName) })
}

function pickNamedPiece(name) {
    var currentPickedPiece = null
    if (gameView.pickedPiece)
        currentPickedPiece = gameView.pickedPiece.pieceModel
    var pieceModel = gameModel.pickNamedPiece(name, currentPickedPiece)
    if (pieceModel)
        gameView.pickPieceAtBoard(gameView.findPiece(pieceModel))
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
    if (gameView.pickedPiece)
        currentPickedPiece = gameView.pickedPiece.pieceModel
    var pieceModel = gameModel.previousPiece(currentPickedPiece)
    if (pieceModel)
        gameView.pickPieceAtBoard(gameView.findPiece(pieceModel))
}

function quit() {
    if (gameModel.checkAutosaveModifiedOutside()) {
        if (! gameModel.isModified)
            return true
        showQuestion(qsTr("Autosaved game was changed by another instance of Pentobi. Overwrite?"),
                     autoSaveNoVerifyAndQuit)
        return false
    }
    autoSaveNoVerify()
    if (isAndroid)
        androidUtils.exit()
    return true
}

function ratedGame() {
    verify(ratedGameCheckFirstGame)
}

function ratedGameCheckFirstGame() {
    if (ratingModel.numberGames === 0)
        initialRatingDialog.open()
    else
        ratedGameNoVerify()
}

function ratedGameNoVerify() {
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
    var level = ratingModel.getNextLevel(playerModel.maxLevel)
    playerModel.level = level
    gameModel.newGame()
    //: Player name for game info in rated game. First argument is version of
    //: Pentobi, second argument is level.
    var computerName =
            qsTr("Pentobi %1 (level %2)").arg(Qt.application.version).arg(level)
    //: Player name for game info in rated game.
    var humanName = qsTr("Human")
    gameModel.playerName0 = computerPlays0 ? computerName : humanName
    gameModel.playerName1 = computerPlays1 ? computerName : humanName
    if (gameModel.nuPlayers > 2)
        gameModel.playerName2 = computerPlays2 ? computerName : humanName
    if (gameModel.nuPlayers > 3)
        gameModel.playerName3 = computerPlays3 ? computerName : humanName
    gameModel.event = qsTr("Rated game")
    gameModel.round = ratingModel.numberGames + 1
    gameView.setupMode = false
    gameView.showToPlay()
    gameView.showPieces()
    isRated = true
    analyzeGameModel.clear()
    delayedCheckComputerMove.restart()
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

function recentFilesContains(file) {
    var entries = recentFiles.entries
    var len = entries.length
    for (var i = 0; i < len; ++i)
        if (entries[i].file === file)
            return true
    return false
}

function reloadFile() {
    openFile(gameModel.file, displayName)
}

function save() {
    if (gameModel.checkFileModifiedOutside())
        showQuestion(qsTr("File has been modified by another application. Overwrite?"),
                     saveCurrentFile)
    else
        saveCurrentFile()
}

function saveAs() {
    if (isAndroid) {
        var file = gameModel.file
        var name
        if (ratingModel.getGameNumberOfFile(file) > 0)
            name = file.substring(file.lastIndexOf("/") + 1)
        else if (displayName !== "")
            name = displayName
        else
            name = gameModel.suggestGameFileName("")
        androidUtils.openSaveDialog(file, name)
    } else {
        var dialog = saveDialog.get()
        dialog.selectedFile =
                folder + "/" + gameModel.suggestGameFileName(folder)
        dialog.open()
    }
}

function saveCurrentFile() {
    saveFile(gameModel.file, displayName)
}

function saveFile(file, displayName) {
    if (! gameModel.save(file))
        showInfo(qsTr("Save failed.") + "\n" + gameModel.getError())
    else {
        recentFiles.add(file, displayName)
        rootWindow.displayName = displayName
        showTemporaryMessage(qsTr("File saved"))
    }
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
    gameView.showTemporaryMessage(text)
}

function showVariationInfo() {
    showTemporaryMessage(qsTr("Variation is now %1").arg(gameModel.getVariationInfo()))
}

function showWindow() {
    if (isAndroid) {
        show()
        return
    }
    width = settings.width
    height = settings.height
    x = settings.x
    y = settings.y
    show()
    var maxWidth = rootWindow.Screen.desktopAvailableWidth
    var maxHeight = rootWindow.Screen.desktopAvailableHeight
    if (width < minimumWidth || height < minimumHeight || x < 0 || y < 0
            || x + width >= maxWidth || y + height >= maxHeight)
    {
        width = Math.min(maxWidth, 1164)
        height = Math.min(maxHeight, width * 662 / 1164)
        x = (rootWindow.Screen.width - width) / 2
        y = (rootWindow.Screen.height - height) / 2
    }
}

function truncate() {
    showQuestion(qsTr("Truncate this subtree?"), gameModel.truncate)
}

function truncateChildren() {
    showQuestion(qsTr("Truncate children?"), truncateChildrenNoVerify)
}

function truncateChildrenNoVerify() {
    gameModel.truncateChildren()
    showTemporaryMessage(qsTr("Children truncated"))
}

function undo() {
    gameModel.undo()
}

function verify(callback) {
    if (gameModel.isModified) {
        showQuestion(qsTr("Discard game?"), callback)
        return
    }
    callback()
}
