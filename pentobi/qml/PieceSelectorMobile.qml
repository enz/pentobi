//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceSelectorMobile.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11

Flickable {
    id: root

    property alias pieces0: pieceList0.pieces
    property alias pieces1: pieceList1.pieces
    property alias pieces2: pieceList2.pieces
    property alias pieces3: pieceList3.pieces
    property alias columns: pieceList0.columns
    property alias rowSpacing: pieceList0.rowSpacing
    property alias transitionsEnabled: transition.enabled

    signal piecePicked(var piece)

    flickableDirection: Flickable.VerticalFlick
    contentHeight: Math.max(pieceList0.y + pieceList0.height,
                            pieceList1.y + pieceList1.height,
                            pieceList2.y + pieceList2.height,
                            pieceList3.y + pieceList3.height)
    clip: true

    Behavior on contentY { NumberAnimation { duration: animationDurationFast } }

    PieceList {
        id: pieceList0

        width: root.width
        onPiecePicked: piece => root.piecePicked(piece)
    }
    PieceList {
        id: pieceList1

        width: root.width
        columns: root.columns
        rowSpacing: root.rowSpacing
        onPiecePicked: piece => root.piecePicked(piece)
    }
    PieceList {
        id: pieceList2

        width: root.width
        columns: root.columns
        rowSpacing: root.rowSpacing
        onPiecePicked: piece => root.piecePicked(piece)
    }
    PieceList {
        id: pieceList3

        width: root.width
        columns: root.columns
        rowSpacing: root.rowSpacing
        onPiecePicked: piece => root.piecePicked(piece)
    }

    // States order the piece lists such that the color to play is on top and
    // the colors are in order of play.
    states: [
        State {
            name: "toPlay0"
            when: gameModel.toPlay === 0

            PropertyChanges {
                target: pieceList0
                y: 0.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList1
                y: pieceList0.height + 1.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList2
                y: pieceList0.height + pieceList1.height + 2.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList3
                y: pieceList0.height + pieceList1.height + pieceList2.height
                   + 3.5 * rowSpacing
            }
        },
        State {
            name: "toPlay1"
            when: gameModel.toPlay === 1

            PropertyChanges {
                target: pieceList1
                y: 0.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList2
                y: pieceList1.height + 1.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList3
                y: pieceList1.height + pieceList2.height + 2.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList0
                y: {
                    if (gameModel.nuColors === 2)
                        return pieceList1.height + 1.5 * rowSpacing
                    if (gameModel.nuColors === 3)
                        return pieceList1.height + pieceList2.height
                                + 2.5 * rowSpacing
                    return pieceList1.height + pieceList2.height
                            + pieceList3.height + 3.5 * rowSpacing
                }
            }
        },
        State {
            name: "toPlay2"
            when: gameModel.toPlay === 2

            PropertyChanges {
                target: pieceList2
                y: 0.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList3
                y: pieceList2.height + 1.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList0
                y: {
                    if (gameModel.nuColors === 3)
                        return pieceList2.height + pieceList3.height
                                + 2.5 * rowSpacing
                    return pieceList2.height + pieceList3.height
                            + 2.5 * rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList1
                y: {
                    if (gameModel.nuColors === 3)
                        return pieceList2.height + pieceList0.height
                                + 2.5 * rowSpacing
                    return pieceList2.height + pieceList3.height
                            + pieceList0.height + 3.5 * rowSpacing
                }
            }
        },
        State {
            name: "toPlay3"
            when: gameModel.toPlay === 3

            PropertyChanges {
                target: pieceList3
                y: 0.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList0
                y: pieceList3.height + 1.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList1
                y: pieceList3.height + pieceList0.height + 2.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList2
                y: pieceList3.height + pieceList0.height + pieceList1.height
                   + 3.5 * rowSpacing
            }
        }
    ]
    transitions:
        Transition {
            id: transition

            SequentialAnimation {
                // Delay showing new color because of piece placement animation
                PauseAnimation {
                    duration:
                        Math.max(animationDurationMove - animationDurationFast,
                                 0)
                }
                NumberAnimation {
                    target: root
                    property: "opacity"
                    to: 0
                    duration: animationDurationFast
                }
                PropertyAction { property: "y" }
                PropertyAction { target: root; property: "contentY"; value: 0 }
                NumberAnimation {
                    target: root
                    property: "opacity"
                    to: 1
                    duration: animationDurationFast
                }
            }
    }
}
