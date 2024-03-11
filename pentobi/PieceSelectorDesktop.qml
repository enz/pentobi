//-----------------------------------------------------------------------------
/** @file pentobi/PieceSelectorDesktop.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Item {
    id: root

    property alias pieces0: pieceList0.pieces
    property alias pieces1: pieceList1.pieces
    property alias pieces2: pieceList2.pieces
    property alias pieces3: pieceList3.pieces
    property alias columns: pieceList0.columns
    // Dummy for compatibility with PieceSelectorMobile
    property bool transitionsEnabled

    signal piecePicked(var piece)

    property real toPlayIndicatorWidth:
        Math.max(Math.min(parent.width / columns, parent.height / 8.3) / 10, 2)

    Row {
        // Set size such that width/height ratio fits the number of columns,
        // taking toPlayIndicator and column spacing into account
        width: Math.min(parent.width - toPlayIndicatorWidth,
                        parent.height / 8.3 * columns)
        height:
            Math.min(parent.height,
                     (parent.width - toPlayIndicatorWidth) / columns * 8.3)
        anchors.centerIn: parent

        Rectangle {
            id: toPlayIndicator

            opacity: gameModel.isGameOver ? 0 : 0.3
            x: 0
            width: toPlayIndicatorWidth
            radius: width / 2
            color: theme.colorText
        }
        Column {
            id: column

            width: parent.width - toPlayIndicatorWidth
            spacing: parent.height / 8.3 * 0.1

            PieceList {
                id: pieceList0

                width: parent.width
                columns: pieces0 ? Math.ceil(pieces0.length / 2) : 11
                onPiecePicked: piece => root.piecePicked(piece)
            }
            PieceList {
                id: pieceList1

                width: parent.width
                columns: root.columns
                onPiecePicked: piece => root.piecePicked(piece)
            }
            PieceList {
                id: pieceList2

                width: parent.width
                columns: root.columns
                onPiecePicked: piece => root.piecePicked(piece)
            }
            PieceList {
                id: pieceList3

                width: parent.width
                columns: root.columns
                onPiecePicked: piece => root.piecePicked(piece)
            }
        }
    }

    // It would be much simpler to use bindings for y/height and a Behavior for
    // the y animation, but I haven't found a way to disable the animation if a
    // game loaded at startup has toPlay != 0
    states: [
        State {
            name: "toPlay0"
            when: gameModel.toPlay === 0

            PropertyChanges {
                target: toPlayIndicator
                y: column.mapToItem(parent, 0, pieceList0.y).y
                height: pieceList0.height
            }
        },
        State {
            name: "toPlay1"
            when: gameModel.toPlay === 1

            PropertyChanges {
                target: toPlayIndicator
                y: column.mapToItem(parent, 0, pieceList1.y).y
                height: pieceList1.height
            }
        },
        State {
            name: "toPlay2"
            when: gameModel.toPlay === 2

            PropertyChanges {
                target: toPlayIndicator
                y: column.mapToItem(parent, 0, pieceList2.y).y
                height: pieceList2.height
            }
        },
        State {
            name: "toPlay3"
            when: gameModel.toPlay === 3

            PropertyChanges {
                target: toPlayIndicator
                y: column.mapToItem(parent, 0, pieceList3.y).y
                height: pieceList0.height
            }
        }
    ]
}
