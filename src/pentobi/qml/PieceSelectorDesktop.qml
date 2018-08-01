//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceSelectorDesktop.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property alias pieces0: pieceList0.pieces
    property alias pieces1: pieceList1.pieces
    property alias pieces2: pieceList2.pieces
    property alias pieces3: pieceList3.pieces
    property int columns: pieces0 ? Math.ceil(pieces0.length / 2) : 11

    property bool transitionsEnabled

    signal piecePicked(var piece)

    property real toPlayIndicatorWidth:
        Math.max(Math.min(parent.width / columns, parent.height / 8.15) / 10, 2)

    Row {
        // Set size sich that width/height ration fits the number of columns,
        // taking toPlayIndicator and column spacing into account
        width: Math.min(parent.width - toPlayIndicatorWidth,
                        parent.height / 8.15 * columns)
        height: Math.min(parent.height,
                         (parent.width - toPlayIndicatorWidth) / columns * 8.15)
        anchors.centerIn: parent

        Rectangle {
            opacity: gameModel.isGameOver ? 0 : 0.7 * theme.opacitySubduedText
            x: 0
            y:
                switch (gameModel.toPlay) {
                case 0: return column.mapToItem(parent, 0, pieceList0.y).y
                case 1: return column.mapToItem(parent, 0, pieceList1.y).y
                case 2: return column.mapToItem(parent, 0, pieceList2.y).y
                case 3: return column.mapToItem(parent, 0, pieceList3.y).y
                }
            width: toPlayIndicatorWidth
            height:
                switch (gameModel.toPlay) {
                case 0: return pieceList0.height
                case 1: return pieceList1.height
                case 2: return pieceList2.height
                case 3: return pieceList3.height
                }
            radius: width / 2
            color: theme.colorText
            Behavior on y {
                NumberAnimation {
                    duration: transitionsEnabled ? 80 : 0
                    easing.type: Easing.InSine
                }
            }
        }
        Column {
            id: column

            width: parent.width - toPlayIndicatorWidth
            spacing: height / 8 * 0.05

            PieceList {
                id: pieceList0

                width: parent.width
                columns: root.columns
                onPiecePicked: root.piecePicked(piece)
            }
            PieceList {
                id: pieceList1

                width: parent.width
                columns: root.columns
                onPiecePicked: root.piecePicked(piece)
            }
            PieceList {
                id: pieceList2

                width: parent.width
                columns: root.columns
                onPiecePicked: root.piecePicked(piece)
            }
            PieceList {
                id: pieceList3

                width: parent.width
                columns: root.columns
                onPiecePicked: root.piecePicked(piece)
            }
        }
    }
}
