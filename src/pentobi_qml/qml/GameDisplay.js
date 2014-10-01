//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/GameDisplay.js
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

function createPieces() {
    destroyPieces(_pieces0)
    destroyPieces(_pieces1)
    destroyPieces(_pieces2)
    destroyPieces(_pieces3)
    var component = Qt.createComponent("Piece.qml")
    if (component.status != Component.Ready)
        throw "Could not create component Piece.qml"
    _pieces0 = createColorPieces(component, boardModel.pieceModels0)
    _pieces1 = createColorPieces(component, boardModel.pieceModels1)
    _pieces2 = createColorPieces(component, boardModel.pieceModels2)
    _pieces3 = createColorPieces(component, boardModel.pieceModels3)
}

function createColorPieces(component, pieceModels) {
    if (pieceModels.length == 0)
        return []
    var pieces = []
    var gameVariant = boardModel.gameVariant
    var colorName
    if (gameVariant == "duo" || gameVariant == "junior")
        switch (pieceModels[0].color) {
        case 0: colorName = "blue"; break
        default: colorName = "green"; break
        }
    else
        switch (pieceModels[0].color) {
        case 0: colorName = "blue"; break
        case 1: colorName = "yellow"; break
        case 2: colorName = "red"; break
        default: colorName = "green"; break
        }
    var properties = {
        "isTrigon": (gameVariant.indexOf("trigon") >= 0),
        "colorName": colorName,
        "gridElementWidth": 0,
        "gridElementHeight": 0,
        "isPicked": Qt.binding(function() { return (this == pickedPiece) }),
        "parentPieceManipulator": pieceManipulator,
        "parentBoard": board,
        "parentAnimationVia": contentItem
    }
    for (var i = 0; i < pieceModels.length; ++i) {
        properties["pieceModel"] = pieceModels[i]
        var piece = component.createObject(root, properties)
        if (piece == null)
            throw "Could not create object Piece"
        pieces.push(piece)
    }
    return pieces
}

function destroyPieces(pieces) {
    if (pieces == null)
        return
    for (var i = 0; i < pieces.length; ++i)
        pieces[i].visible = false
    for (i = 0; i < pieces.length; ++i)
        pieces[i].destroy()
}

function findPiece(pieceModel) {
    var i
    for (i = 0; i < _pieces0.length; ++i)
        if (_pieces0[i].pieceModel === pieceModel)
            return _pieces0[i]
    for (i = 0; i < _pieces1.length; ++i)
        if (_pieces1[i].pieceModel === pieceModel)
            return _pieces1[i]
    if (boardModel.nuColors > 2)
        for (i = 0; i < _pieces2.length; ++i)
            if (_pieces2[i].pieceModel === pieceModel)
                return _pieces2[i]
    if (boardModel.nuColors > 3)
        for (i = 0; i < _pieces3.length; ++i)
            if (_pieces3[i].pieceModel === pieceModel)
                return _pieces3[i]
    return null
}

function pickPiece(piece) {
    if (piece.pieceModel.color != boardModel.toPlay
            || boardModel.isGameOver)
        return
    if (! pieceManipulator.visible) {
        // Position pieceManipulator at center of piece if possible, but
        // make sure it is completely visible
        var newCoord = root.mapFromItem(piece, 0, 0)
        var x = newCoord.x - pieceManipulator.width / 2
        var y = newCoord.y - pieceManipulator.height / 2
        x = Math.max(Math.min(x, root.width - pieceManipulator.width), 0)
        y = Math.max(Math.min(y, root.height - pieceManipulator.height), 0)
        pieceManipulator.x = x
        pieceManipulator.y = y
    }
    pickedPiece = piece
}

function showMoveHint(move) {
    var pieceModel = boardModel.preparePiece(boardModel.toPlay, move)
    var piece = findPiece(pieceModel)
    var pos = board.mapToItem(pieceManipulator.parent,
                              board.mapFromGameX(pieceModel.gameCoord.x),
                              board.mapFromGameY(pieceModel.gameCoord.y))
    pieceManipulator.x = pos.x - pieceManipulator.width / 2
    pieceManipulator.y = pos.y - pieceManipulator.height / 2
    pieceManipulator.legal = true
    pickedPiece = piece
}
