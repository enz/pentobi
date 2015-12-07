function createPieces() {
    destroyPieces()
    var file = (gameModel.gameVariant.indexOf("trigon") >= 0) ?
                "PieceTrigon.qml" : "PieceClassic.qml"
    var component = Qt.createComponent(file)
    if (component.status !== Component.Ready)
        throw "Could not create component " + file
    _pieces0 = createColorPieces(component, gameModel.pieceModels0)
    _pieces1 = createColorPieces(component, gameModel.pieceModels1)
    _pieces2 = createColorPieces(component, gameModel.pieceModels2)
    _pieces3 = createColorPieces(component, gameModel.pieceModels3)
}

function createColorPieces(component, pieceModels) {
    if (pieceModels.length === 0)
        return []
    var colorName
    switch (pieceModels[0].color) {
    case 0: colorName = "blue"; break
    case 1:
        colorName = gameModel.gameVariant == "duo" ||
                gameModel.gameVariant == "junior" ? "green" : "yellow"; break
    case 2: colorName = "red"; break
    case 3: colorName = "green"; break
    }
    var properties = {
        "colorName": colorName,
        "isPicked": Qt.binding(function() { return this === pickedPiece }) }
    var pieces = []
    for (var i = 0; i < pieceModels.length; ++i) {
        properties["pieceModel"] = pieceModels[i]
        pieces.push(component.createObject(this, properties))
    }
    return pieces
}

function destroyColorPieces(pieces) {
    if (pieces === undefined)
        return
    for (var i = 0; i < pieces.length; ++i) {
        // If visible is not set to false before destroying the item, sometimes
        // it stays on the screen (happened with Qt 5.5 on Linux).
        pieces[i].visible = false
        pieces[i].destroy()
    }
}

function destroyPieces() {
    destroyColorPieces(_pieces0)
    destroyColorPieces(_pieces1)
    destroyColorPieces(_pieces2)
    destroyColorPieces(_pieces3)
}

function findPiece(pieceModel, color) {
    var pieces
    switch (color) {
    case 0: pieces = _pieces0; break
    case 1: pieces = _pieces1; break
    case 2: pieces = _pieces2; break
    case 3: pieces = _pieces3; break
    }
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
    pieceTransitionsEnabled = true
    pickedPiece = piece
}

function showMoveHint(move) {
    var pieceModel = gameModel.preparePiece(gameModel.toPlay, move)
    var piece = findPiece(pieceModel, gameModel.toPlay)
    var pos = board.mapToItem(pieceManipulator.parent,
                              board.mapFromGameX(pieceModel.gameCoord.x),
                              board.mapFromGameY(pieceModel.gameCoord.y))
    pieceManipulator.x = pos.x - pieceManipulator.width / 2
    pieceManipulator.y = pos.y - pieceManipulator.height / 2
    pickedPiece = piece
}
