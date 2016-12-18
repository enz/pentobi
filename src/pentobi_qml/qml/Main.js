function about() {
    var url = "http://pentobi.sourceforge.net"
    showInfo("<h2>" + qsTr("Pentobi") + "</h2><p>" +
             qsTr("Version %1").arg(Qt.application.version) + "</p><p>" +
             qsTr("Computer opponent for the board game Blokus.") + "<br>" +
             qsTr("&copy; 2011&ndash;%1 Markus&nbsp;Enzenberger").arg(2016) +
             "<br><a href=\"" + url + "\">" + url + "</a></p>")
}

function analyzeGame() {
    if (! gameModel.isMainVar) {
        showInfo(qsTr("Game analysis is only possible in main variation."))
        return
    }
    gameDisplay.showAnalyzeGame()
    analyzeGameModel.start(gameModel, playerModel)
}

function autoSave() {
    wasGenMoveRunning =
            playerModel.isGenMoveRunning && ! isMoveHintRunning
            && ! isPlaySingleMoveRunning
    gameModel.autoSave()
    analyzeGameModel.autoSave(gameModel)
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
        analyzeGameModel.clear()
        initComputerColors()
    })
}

function checkComputerMove() {
    if (gameModel.isGameOver) {
        showInfo(gameModel.getResultMessage())
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

/** If the computer already plays the current color to play, start generating
    a move; if he doesn't, make him play the current color (and only the
    current color). */
function computerPlay() {
    if (playerModel.isGenMoveRunning)
        return
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
                    (variant == "classic_2" || variant == "trigon_2"
                     || variant == "nexos_2")
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

function createTheme(themeName) {
    var source = "qrc:///qml/themes/" + themeName + "/Theme.qml"
    return Qt.createComponent(source).createObject(root)
}

function deleteAllVar() {
    showQuestion(qsTr("Delete all variations?"), gameModel.deleteAllVar)
}

function exportAsciiArt(fileUrl) {
    if (! gameModel.saveAsciiArt(getFileFromUrl(fileUrl)))
        showError(qsTr("Save failed.") + "\n" + gameModel.lastInputOutputError)
}

function exportImage(fileUrl) {
    if (! gameDisplay.grabBoardToImage(function(result) {
        if (! result.saveToFile(getFileFromUrl(fileUrl)))
            showError(qsTr("Saving image failed."))
    }, exportImageWidth))
        showError(qsTr("Creating image failed."))
}

function genMove() {
    gameDisplay.pickedPiece = null
    isMoveHintRunning = false
    playerModel.startGenMove(gameModel)
}

function getFileFromUrl(fileUrl) {
    var file = fileUrl.toString()
    file = file.replace(/^(file:\/{3})/,"/")
    return decodeURIComponent(file)
}

function getTitle(file, isModified) {
    if (file === "")
        return qsTr("Pentobi")
    var pos = Math.max(file.lastIndexOf("/"), file.lastIndexOf("\\"));
    return (isModified ? "*" : "") + file.substring(pos + 1)
}

function init() {
    // Settings might contain unusable geometry
    var maxWidth = Screen.desktopAvailableWidth
    var maxHeight = Screen.desktopAvailableHeight
    if (x < 0 || x + width > maxWidth || y < 0 || y + height > maxHeight) {
        if (width > maxWidth || height > Screen.maxHeight) {
            width = defaultWidth
            height = defaultHeight
        }
        x = (maxWidth - width) / 2
        y = (maxHeight - height) / 2
    }
    if (! gameModel.loadAutoSave()) {
        gameDisplay.createPieces()
        initComputerColors()
        return
    }
    gameDisplay.createPieces()
    if (gameModel.checkFileModifiedOutside())
    {
        showQuestion(qsTr("File has been modified by another application. Reload?"), reloadFile)
        return
    }
    analyzeGameModel.loadAutoSave(gameModel)
    if (wasGenMoveRunning)
        checkComputerMove()
}

function initComputerColors() {
    // Default setting is that the computer plays all colors but the first
    computerPlays0 = false
    computerPlays1 = true
    computerPlays2 = true
    computerPlays3 = true
    if (gameModel.gameVariant == "classic_2"
            || gameModel.gameVariant == "trigon_2"
            || gameModel.gameVariant == "nexos_2")
        computerPlays2 = false
}

function isComputerToPlay() {
    if (gameModel.gameVariant == "classic_3" && gameModel.toPlay == 3)
        return computerPlays(gameModel.altPlayer)
    return computerPlays(gameModel.toPlay)
}

function keepOnlyPosition() {
    showQuestion(qsTr("Keep only position?"), gameModel.keepOnlyPosition())
}

function keepOnlySubtree() {
    showQuestion(qsTr("Keep only subtree?"), gameModel.keepOnlySubtree())
}

function moveGenerated(move) {
    if (isMoveHintRunning) {
        gameDisplay.showMoveHint(move)
        isMoveHintRunning = false
        return
    }
    gameModel.playMove(move)
    if (isPlaySingleMoveRunning)
        isPlaySingleMoveRunning = false
    else
        delayedCheckComputerMove.restart()
}

function moveHint() {
    if (gameModel.isGameOver)
        return
    isMoveHintRunning = true
    playerModel.startGenMoveAtLevel(gameModel, 1)
}

function newGameNoVerify()
{
    gameModel.newGame()
    gameDisplay.showToPlay()
    analyzeGameModel.clear()
    initComputerColors()
}

function newGame()
{
    verify(newGameNoVerify)
}

function open() {
    verify(openNoVerify)
}

function openNoVerify() {
    openDialog.open()
}

function openFile(file) {
    gameDisplay.destroyPieces()
    if (! gameModel.open(file))
        showError(qsTr("Open failed.") + "\n" + gameModel.lastInputOutputError)
    else {
        computerPlays0 = false
        computerPlays1 = false
        computerPlays2 = false
        computerPlays3 = false
    }
    gameDisplay.createPieces()
    gameDisplay.showToPlay()
    analyzeGameModel.clear()
}

function openFileUrl() {
    openFile(getFileFromUrl(openDialog.item.fileUrl))
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

function play(pieceModel, gameCoord) {
    var wasComputerToPlay = isComputerToPlay()
    gameModel.playPiece(pieceModel, gameCoord)
    // We don't continue automatic play if the human played a move for a color
    // played by the computer.
    if (! wasComputerToPlay)
        delayedCheckComputerMove.restart()
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

function saveCurrentFile() {
    saveFile(gameModel.file)
}

function saveFile(file) {
    if (! gameModel.save(file))
        showError(qsTr("Save failed.") + "\n" + gameModel.lastInputOutputError)
}

function saveFileUrl(fileUrl) {
    saveFile(getFileFromUrl(fileUrl))
}

function showComputerColorDialog() {
    if (computerColorDialogLoader.status === Loader.Null)
        computerColorDialogLoader.sourceComponent =
                computerColorDialogComponent
    var dialog = computerColorDialogLoader.item
    dialog.computerPlays0 = computerPlays0
    dialog.computerPlays1 = computerPlays1
    dialog.computerPlays2 = computerPlays2
    dialog.computerPlays3 = computerPlays3
    dialog.open()
}

function showError(text) {
    if (errorMessageLoader.status === Loader.Null)
        errorMessageLoader.sourceComponent = errorMessageComponent
    var dialog = errorMessageLoader.item
    dialog.text = text
    dialog.open()
}

function showInfo(text) {
    if (infoMessageLoader.status === Loader.Null)
        infoMessageLoader.sourceComponent = infoMessageComponent
    var dialog = infoMessageLoader.item
    dialog.text = text
    dialog.open()
}

function showQuestion(text, acceptedFunc) {
    if (questionMessageLoader.status === Loader.Null)
        questionMessageLoader.sourceComponent = questionMessageComponent
    var dialog = questionMessageLoader.item
    dialog.text = text
    dialog.accepted.connect(acceptedFunc)
    dialog.open()
}

function truncate() {
    showQuestion(qsTr("Truncate this subtree?"), gameModel.truncate)
}

function truncateChildren() {
    showQuestion(qsTr("Truncate children?"), gameModel.truncateChildren)
}

function undo() {
    gameModel.undo()
}

function verify(callback)
{
    if (gameModel.file !== "") {
        if (gameModel.isModified) {
            showQuestion(qsTr("Discard changes?"), callback)
            return
        }
    }
    else if (! gameModel.isGameEmpty && ! gameModel.isGameOver) {
        showQuestion(qsTr("Abort game?"), callback)
        return
    }
    callback()
}
