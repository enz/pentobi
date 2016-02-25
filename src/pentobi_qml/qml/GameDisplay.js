function createColorPieces(component, pieceModels) {
    if (pieceModels.length === 0)
        return []
    var colorName
    switch (pieceModels[0].color) {
    case 0: colorName = "blue"; break
    case 1:
        colorName = gameModel.gameVariant == "duo"
                || gameModel.gameVariant == "junior"
                || gameModel.gameVariant == "callisto_2" ?
                    "green" : "yellow"; break
    case 2: colorName = "red"; break
    case 3: colorName = "green"; break
    }
    var properties = {
        "colorName": colorName,
        "isPicked": Qt.binding(function() { return this === pickedPiece }),
        "isMarked": Qt.binding(function() {
            return markLastMove && this.pieceModel.isLastMove })
    }
    var pieces = []
    for (var i = 0; i < pieceModels.length; ++i) {
        properties["pieceModel"] = pieceModels[i]
        pieces.push(component.createObject(this, properties))
    }
    return pieces
}

function createPieces() {
    var file
    if (gameModel.gameVariant.lastIndexOf("trigon") === 0)
        file = "PieceTrigon.qml"
    else if (gameModel.gameVariant.lastIndexOf("nexos") === 0)
        file = "PieceNexos.qml"
    else if (gameModel.gameVariant.lastIndexOf("callisto") === 0)
        file = "PieceCallisto.qml"
    else
        file = "PieceClassic.qml"
    var component = Qt.createComponent(file)
    if (component.status !== Component.Ready)
        throw "Could not create component " + file
    _pieces0 = createColorPieces(component, gameModel.pieceModels0)
    _pieces1 = createColorPieces(component, gameModel.pieceModels1)
    _pieces2 = createColorPieces(component, gameModel.pieceModels2)
    _pieces3 = createColorPieces(component, gameModel.pieceModels3)
}

function destroyColorPieces(pieces) {
    if (pieces === undefined)
        return
    for (var i = 0; i < pieces.length; ++i) {
        pieces[i].visible = false
        pieces[i].destroy(1000)
    }
}

function destroyPieces() {
    dropPiece()
    destroyColorPieces(_pieces0); _pieces0 = []
    destroyColorPieces(_pieces1); _pieces1 = []
    destroyColorPieces(_pieces2); _pieces2 = []
    destroyColorPieces(_pieces3); _pieces3 = []
}

function findPiece(pieceModel, color) {
    var pieces
    switch (color) {
    case 0: pieces = _pieces0; break
    case 1: pieces = _pieces1; break
    case 2: pieces = _pieces2; break
    case 3: pieces = _pieces3; break
    }
    if (pieces === undefined)
        return null // Pieces haven't been created yet
    for (var i = 0; i < pieces.length; ++i)
        if (pieces[i].pieceModel === pieceModel)
            return pieces[i]
    return null
}

function pickPiece(piece) {
    if (playerModel.isGenMoveRunning || gameModel.isGameOver
            || piece.pieceModel.color !== gameModel.toPlay)
        return
    if (! pieceManipulator.visible) {
        // Position pieceManipulator at center of piece if possible, but
        // make sure it is completely visible
        var newCoord = mapFromItem(piece, 0, 0)
        var x = newCoord.x - pieceManipulator.width / 2
        var y = newCoord.y - pieceManipulator.height / 2
        x = Math.max(Math.min(x, width - pieceManipulator.width), 0)
        y = Math.max(Math.min(y, height - pieceManipulator.height), 0)
        pieceManipulator.x = x
        pieceManipulator.y = y
    }
    transitionsEnabled = true
    pickedPiece = piece
}

function showMoveHint(move) {
    var pieceModel = gameModel.preparePiece(gameModel.toPlay, move)
    var pos = board.mapToItem(pieceManipulator.parent,
                              board.mapFromGameX(pieceModel.gameCoord.x),
                              board.mapFromGameY(pieceModel.gameCoord.y))
    pieceManipulator.x = pos.x - pieceManipulator.width / 2
    pieceManipulator.y = pos.y - pieceManipulator.height / 2
    transitionsEnabled = true
    pickedPiece = findPiece(pieceModel, gameModel.toPlay)
    transitionsEnabled = false
}
