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

    // We want to show the unplayed pieces in slightly less bright colors
    // than the pieces on the board, but not if colorBackground is light
    // otherwise the contrast to the yellow pieces is not strong enough.
    opacity: Math.min(0.94 + 0.08 * theme.colorBackground.hslLightness, 1)

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
