//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceList.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Grid {
    id: root

    property var pieces

    signal piecePicked(var piece)

    // Show unplayed pieces in slightly less bright colors, such that they
    // don't distract from the pieces on board, but not in desktop mode, where
    // the unplayed pieces are relatively small, or if the background is bright
    // to avoid bad contrast with yellow pieces.
    opacity:
        isDesktop ? 1 :
                    Math.min(0.9 + 0.1 * theme.colorBackground.hslLightness, 1)

    Repeater {
        model: pieces

        MouseArea {
            id: mouseArea

            width: root.width / columns; height: width
            visible: ! modelData.pieceModel.isPlayed
            onClicked: {
                gameDisplay.dropCommentFocus()
                piecePicked(modelData)
            }
            Component.onCompleted: modelData.parentUnplayed = mouseArea
        }
    }
}
