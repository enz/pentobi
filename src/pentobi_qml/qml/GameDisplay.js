//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/GameDisplay.js
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

function createPieces(pieceModels) {
    if (pieceModels.length == 0)
        return
    var comp = Qt.createComponent("Piece.qml")
    if (comp.status != Component.Ready)
        throw "Could not create component Piece.qml"
    var pieces = []
    var imageSourceWidth = board.gridElementWidth
    var imageSourceHeight = board.gridElementHeight
    var gameVariant = boardModel.gameVariant
    var isTrigon = (gameVariant.indexOf("trigon") >= 0)
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
    for (var i = 0; i < pieceModels.length; ++i) {
        var piece =
                comp.createObject(root,
                                  {
                                      "pieceModel": pieceModels[i],
                                      "isTrigon": isTrigon,
                                      "colorName": colorName,
                                      "gridElementWidth": 0,
                                      "gridElementHeight": 0,
                                      "imageSourceWidth":  imageSourceWidth,
                                      "imageSourceHeight": imageSourceHeight,
                                      "isPicked": Qt.binding(function() {
                                          return (this == pickedPiece) }),
                                      "parentPieceManipulator":
                                      pieceManipulator,
                                      "parentBoard": board,
                                      "parentAnimationVia": contentItem
                                  })
        if (piece == null)
            throw "Could not create object Piece"
        pieces.push(piece)
    }
    return pieces
}

function destroyPieces(pieces) {
    if (pieces != null)
        for (var i = 0; i < pieces.length; ++i)
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
    if (nuColors > 2)
        for (i = 0; i < _pieces2.length; ++i)
            if (_pieces2[i].pieceModel === pieceModel)
                return _pieces2[i]
    if (nuColors > 3)
        for (i = 0; i < _pieces3.length; ++i)
            if (_pieces3[i].pieceModel === pieceModel)
                return _pieces3[i]
    return null
}

function pickPiece(piece) {
    if (piece.pieceModel.color != boardModel.toPlay)
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
    pieceManipulator.x =
            board.x + board.mapFromGameX(pieceModel.gameCoord.x) -
            pieceManipulator.width / 2
    pieceManipulator.y =
            board.y + board.mapFromGameY(pieceModel.gameCoord.y) -
            pieceManipulator.height / 2
    pieceManipulator.legal = true
    pickedPiece = piece
}
