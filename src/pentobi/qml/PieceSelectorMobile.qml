//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceSelectorMobile.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11

Flickable {
    id: root

    property int toPlay
    property alias pieces0: pieceList0.pieces
    property alias pieces1: pieceList1.pieces
    property alias pieces2: pieceList2.pieces
    property alias pieces3: pieceList3.pieces
    property int nuColors
    property int columns
    property real rowSpacing
    property alias transitionsEnabled: transition.enabled

    signal piecePicked(var piece)

    flickableDirection: Flickable.VerticalFlick
    contentHeight: column.height
    clip: true

    Behavior on contentY { NumberAnimation { duration: animationDurationFast } }

    Column {
        id: column

        spacing: root.rowSpacing
        topPadding: 0.5 * root.rowSpacing
        bottomPadding: 0.5 * root.rowSpacing

        PieceList {
            id: pieceList0

            width: root.width
            columns: root.columns
            rowSpacing: root.rowSpacing
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            id: pieceList1

            width: root.width
            columns: root.columns
            rowSpacing: root.rowSpacing
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            id: pieceList2

            width: root.width
            columns: root.columns
            rowSpacing: root.rowSpacing
            onPiecePicked: root.piecePicked(piece)
        }
        PieceList {
            id: pieceList3

            width: root.width
            columns: root.columns
            rowSpacing: root.rowSpacing
            onPiecePicked: root.piecePicked(piece)
        }
    }

    // States order the piece lists such that the color to play is on top. If a
    // player plays two colors, their second color follows, such that at least
    // at the end of the game all of their remaining pieces should be in the
    // visible area. Otherwise the colors are in order of play.
    states: [
        State {
            name: "toPlay0"
            when: toPlay === 0

            PropertyChanges {
                target: pieceList0
                y: 0.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList1
                y: {
                    if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                        return pieceList0.height + pieceList2.height + 1.5 * rowSpacing
                    return pieceList0.height + rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList2
                y: {
                    if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                        return pieceList0.height + rowSpacing
                    return pieceList0.height + pieceList1.height  + 1.5 * rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList3
                y: pieceList0.height + pieceList1.height + pieceList2.height + 2 * rowSpacing
            }
        },
        State {
            name: "toPlay1"
            when: toPlay === 1

            PropertyChanges {
                target: pieceList1
                y: 0.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList2
                y: {
                    if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                        return pieceList1.height + pieceList3.height + 1.5 * rowSpacing
                    return pieceList1.height + rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList3
                y: {
                    if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                        return pieceList1.height + rowSpacing
                    return pieceList1.height + pieceList2.height + 1.5 * rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList0
                y: {
                    if (gameModel.nuColors === 2)
                        return pieceList1.height + pieceList2.height + pieceList3.height + rowSpacing
                    if (gameModel.nuColors === 3)
                        return pieceList1.height + pieceList2.height + pieceList3.height + 1.5 * rowSpacing
                    return pieceList1.height + pieceList2.height + pieceList3.height + 2 * rowSpacing
                }
            }
        },
        State {
            name: "toPlay2"
            when: toPlay === 2

            PropertyChanges {
                target: pieceList2
                y: 0.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList3
                y: {
                    if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                        return pieceList2.height + pieceList0.height + 1.5 * rowSpacing
                    return pieceList2.height + rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList0
                y: {
                    if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                        return pieceList2.height + rowSpacing
                    if (gameModel.nuColors === 3)
                        return pieceList2.height + pieceList3.height + rowSpacing
                    return pieceList2.height + pieceList3.height + 1.5 * rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList1
                y: {
                    if (gameModel.nuColors === 3)
                        return pieceList2.height + pieceList3.height + pieceList0.height + 1.5 * rowSpacing
                    return pieceList2.height + pieceList3.height + pieceList0.height + 2 * rowSpacing
                }
            }
        },
        State {
            name: "toPlay3"
            when: toPlay === 3

            PropertyChanges {
                target: pieceList3
                y: 0.5 * rowSpacing
            }
            PropertyChanges {
                target: pieceList0
                y: {
                    if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                        return pieceList3.height + pieceList1.height + 1.5 * rowSpacing
                    return pieceList3.height + rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList1
                y: {
                    if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                        return pieceList3.height + rowSpacing
                    return pieceList3.height + pieceList0.height + 1.5 * rowSpacing
                }
            }
            PropertyChanges {
                target: pieceList2
                y: pieceList3.height + pieceList0.height + pieceList1.height + 2 * rowSpacing
            }
        }
    ]
    transitions:
        Transition {
            id: transition

            SequentialAnimation {
                PropertyAction {
                    target: pieceList0; property: "y"; value: pieceList0.y }
                PropertyAction {
                    target: pieceList1; property: "y"; value: pieceList1.y }
                PropertyAction {
                    target: pieceList2; property: "y"; value: pieceList2.y }
                PropertyAction {
                    target: pieceList3; property: "y"; value: pieceList3.y }
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
                PropertyAction { target: pieceList0; property: "y" }
                PropertyAction { target: pieceList1; property: "y" }
                PropertyAction { target: pieceList2; property: "y" }
                PropertyAction { target: pieceList3; property: "y" }
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
