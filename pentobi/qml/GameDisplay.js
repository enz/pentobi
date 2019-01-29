//-----------------------------------------------------------------------------
/** @file pentobi/qml/GameDisplay.js
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

function createColorPieces(component, pieceModels) {
    if (pieceModels.length === 0)
        return []
    var properties = { }
    var pieces = []
    for (var i = 0; i < pieceModels.length; ++i) {
        properties["pieceModel"] = pieceModels[i]
        pieces.push(component.createObject(gameDisplay, properties))
    }
    return pieces
}

function createPieces() {
    destroyPieces()
    var file
    var gameVariant = gameModel.gameVariant
    if (gameVariant.startsWith("trigon"))
        file = "PieceTrigon.qml"
    else if (gameVariant.startsWith("nexos"))
        file = "PieceNexos.qml"
    else if (gameVariant.startsWith("callisto"))
        file = "PieceCallisto.qml"
    else if (gameVariant.startsWith("gembloq"))
        file = "PieceGembloQ.qml"
    else
        file = "PieceClassic.qml"
    var component = Qt.createComponent(file)
    if (component.status !== Component.Ready)
        console.warn(component.errorString())
    pieces0 = createColorPieces(component, gameModel.getPieceModels(0))
    pieces1 = createColorPieces(component, gameModel.getPieceModels(1))
    pieces2 = createColorPieces(component, gameModel.getPieceModels(2))
    pieces3 = createColorPieces(component, gameModel.getPieceModels(3))
    pieceSelector.transitionsEnabled =
            Qt.binding(function() { return enableAnimations })
}

function destroyColorPieces(pieces) {
    if (pieces === undefined)
        return
    for (var i = 0; i < pieces.length; ++i) {
        pieces[i].visible = false
        pieces[i].destroy()
    }
}

function destroyPieces() {
    pieceSelector.transitionsEnabled = false
    pickedPiece = null
    destroyColorPieces(pieces0); pieces0 = []
    destroyColorPieces(pieces1); pieces1 = []
    destroyColorPieces(pieces2); pieces2 = []
    destroyColorPieces(pieces3); pieces3 = []
}

function dropPieceFast() {
    if (! pickedPiece)
        return
    var old = enableAnimations
    enableAnimations = false
    pickedPiece = null
    enableAnimations = old
}

function findPiece(pieceModel) {
    var pieces
    switch (pieceModel.color) {
    case 0: pieces = pieces0; break
    case 1: pieces = pieces1; break
    case 2: pieces = pieces2; break
    case 3: pieces = pieces3; break
    }
    if (pieces === undefined)
        return null // Pieces haven't been created yet
    for (var i = 0; i < pieces.length; ++i)
        if (pieces[i].pieceModel === pieceModel)
            return pieces[i]
    return null
}

function movePiece(x, y) {
    if (pickedPiece == null)
        return
    var pos = pieceManipulator.mapToItem(
                board, pieceManipulator.width / 2, pieceManipulator.height / 2)
    var fastMove
    if (! board.contains(pos)) {
        // Outside board before moving, move to center of board
        pos = mapFromItem(board, board.width / 2, board.height / 2)
        x = pos.x - pieceManipulator.width / 2
        y = pos.y - pieceManipulator.height / 2
        fastMove = false
    }
    else {
        pos = pieceManipulator.mapToItem(
                    board,
                    pieceManipulator.width / 2 + x - pieceManipulator.x,
                    pieceManipulator.height / 2 + y - pieceManipulator.y)
        pos.x = Math.max(0, pos.x)
        pos.x = Math.min(board.width - 1, pos.x)
        pos.y = Math.max(0, pos.y)
        pos.y = Math.min(board.height - 1, pos.y)
        pos = mapFromItem(board, pos.x, pos.y)
        x = pos.x - pieceManipulator.width / 2
        y = pos.y - pieceManipulator.height / 2
        fastMove = true
    }
    pieceManipulator.fastMove = fastMove
    pieceManipulator.x = x
    pieceManipulator.y = y
    pieceManipulator.fastMove = false
}

function onBoardClicked(pos) {
    dropCommentFocus()
    if (! setupMode)
        return
    var pieceModel = gameModel.addEmpty(pos)
    if (! pieceModel)
        return
    var piece = findPiece(pieceModel)
    pos = mapFromItem(piece, (piece.width - pieceManipulator.width) / 2,
                      (piece.height - pieceManipulator.height) / 2)
    pieceManipulator.x = pos.x
    pieceManipulator.y = pos.y
    pickedPiece = piece
}

function onBoardRightClicked(pos) {
    dropCommentFocus()
    var n = gameModel.getMoveNumberAt(pos)
    if (n < 0)
        return
    gameDisplay.openBoardContextMenu(
                n, board.mapFromGameX(pos.x + 0.5),
                board.mapFromGameY(pos.y + 0.5))
}

function shiftPiece(dx, dy) {
    if (gameModel.gameVariant.startsWith("gembloq"))
        // In GembloQ, every piece has at least one full square, so we can use
        // half the x resolution, which makes positioning easier for the user
        movePiece(pieceManipulator.x + dx * board.gridWidth,
                  pieceManipulator.y + dy * board.gridHeight / 2)
    else if (gameModel.gameVariant.startsWith("nexos"))
        movePiece(pieceManipulator.x + dx * board.gridWidth,
                  pieceManipulator.y + dy * board.gridHeight)
    else
        movePiece(pieceManipulator.x + dx * board.gridWidth / 2,
                  pieceManipulator.y + dy * board.gridHeight / 2)
}

function shiftPieceFast(dx, dy) {
    movePiece(pieceManipulator.x + dx * board.width / 4,
              pieceManipulator.y + dy * board.height / 4)
}

function pickPiece(piece) {
    pickPieceAt(piece, mapFromItem(piece, 0, 0))
}

function pickPieceAt(piece, coord) {
    if (playerModel.isGenMoveRunning || gameModel.isGameOver)
        return
    if (piece.pieceModel.color !== gameModel.toPlay && ! setupMode) {
        gameDisplay.showToPlay()
        return
    }
    if (! pieceManipulator.pieceModel) {
        // Position pieceManipulator at center of piece if possible, but
        // make sure it is completely visible
        var x = coord.x - pieceManipulator.width / 2
        var y = coord.y - pieceManipulator.height / 2
        x = Math.max(Math.min(x, width - pieceManipulator.width), 0)
        y = Math.max(Math.min(y, height - pieceManipulator.height), 0)
        pieceManipulator.x = x
        pieceManipulator.y = y
    }
    pickedPiece = piece
}

function pickPieceAtBoard(piece) {
    pickPieceAt(piece, mapFromItem(board, board.width / 2, board.height / 2))
}

function playPickedPiece() {
    if (! pickedPiece)
        return
    var pos = pieceManipulator.mapToItem(board, pieceManipulator.width / 2,
                                         pieceManipulator.height / 2)
    if (! board.contains(pos))
        pickedPiece = null
    else if (setupMode)
        gameModel.addSetup(pieceManipulator.pieceModel, board.mapToGame(pos))
    else if (pieceManipulator.legal)
        play(pieceManipulator.pieceModel, board.mapToGame(pos))
}

function showMove(move) {
    var pieceModel = gameModel.preparePiece(move)
    if (pieceModel === null)
        return
    var newPickedPiece = findPiece(pieceModel)
    if (pickedPiece && newPickedPiece !== pickedPiece)
        pickedPiece = null
    var pos = board.mapToItem(
                pieceManipulator.parent,
                board.mapFromGameX(pieceModel.gameCoord.x),
                board.mapFromGameY(pieceModel.gameCoord.y))
    pieceManipulator.x = pos.x - pieceManipulator.width / 2
    pieceManipulator.y = pos.y - pieceManipulator.height / 2
    pickedPiece = newPickedPiece
}
