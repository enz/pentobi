//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/GameDisplay.js
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

function createPieces(pieceModels) {
    var comp = Qt.createComponent("Piece.qml")
    if (comp.status != Component.Ready)
        throw "Could not create component Piece.qml"
    var pieces = []
    var imageSourceWidth = board.gridElementWidth
    var imageSourceHeight = board.gridElementHeight
    for (var i = 0; i < pieceModels.length; ++i) {
        var piece =
                comp.createObject(root,
                                  {
                                      "pieceModel": pieceModels[i],
                                      "gridElementWidth": 0,
                                      "gridElementHeight": 0,
                                      "imageSourceWidth":  imageSourceWidth,
                                      "imageSourceHeight": imageSourceHeight,
                                      "gameVariant": boardModel.gameVariant,
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

function pickPiece(piece) {
    if (piece.pieceModel.color != boardModel.toPlay) {
        clearPieceSelectorForcedColor()
        return
    }
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
