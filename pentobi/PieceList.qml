//-----------------------------------------------------------------------------
/** @file pentobi/PieceList.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Grid {
    id: root

    property list<PieceBase> pieces

    signal piecePicked(var piece)

    Repeater {
        model: pieces

        MouseArea {
            id: mouseArea

            width: root.width / columns; height: width
            visible: ! modelData.pieceModel.isPlayed
            onClicked: {
                gameView.dropCommentFocus()
                piecePicked(modelData)
            }
            Component.onCompleted: modelData.parentUnplayed = mouseArea
        }
    }
}
