//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceSelectorMobile.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

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

    // States order the piece lists such that the color to play is on top and
    // the colors are in order of play (c=color of piece list, h0-h3=heights of
    // piece lists, d=rowSpacing)
    function getY(toPlay, c, h0, h1, h2, h3, d) {
        switch (toPlay) {
        case 0:
            switch (c) {
            case 0: return 0.5 * d
            case 1: return h0 + 1.5 * d
            case 2: return h0 + h1 + 2.5 * d
            case 3: return h0 + h1 + h2 + 3.5 * d
            }
            break
        case 1:
            switch (c) {
            case 1: return 0.5 * d
            case 2: return h1 + 1.5 * d
            case 3: return h1 + h2 + 2.5 * d
            case 0: return h1 + h2 + h3 + 3.5 * d
            }
            break
        case 2:
            switch (c) {
            case 2: return 0.5 * d
            case 3: return h2 + 1.5 * d
            case 0: return h2 + h3 + 2.5 * d
            case 1: return h2 + h3 + h0 + 3.5 * d
            }
            break
        case 3:
            switch (c) {
            case 3: return 0.5 * d
            case 0: return h3 + 1.5 * d
            case 1: return h3 + h0 + 2.5 * d
            case 2: return h3 + h0 + h1 + 3.5 * d
            }
            break
        }
    }

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
                y: getY(0, 0, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList1
                y: getY(0, 1, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList2
                y: getY(0, 2, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList3
                y: getY(0, 3, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
        },
        State {
            name: "toPlay1"
            when: gameModel.toPlay === 1

            PropertyChanges {
                target: pieceList0
                y: getY(1, 0, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList1
                y: getY(1, 1, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList2
                y: getY(1, 2, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList3
                y: getY(1, 3, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
        },
        State {
            name: "toPlay2"
            when: gameModel.toPlay === 2

            PropertyChanges {
                target: pieceList0
                y: getY(2, 0, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList1
                y: getY(2, 1, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList2
                y: getY(2, 2, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList3
                y: getY(2, 3, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
        },
        State {
            name: "toPlay3"
            when: gameModel.toPlay === 3

            PropertyChanges {
                target: pieceList0
                y: getY(3, 0, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList1
                y: getY(3, 1, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList2
                y: getY(3, 2, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
            }
            PropertyChanges {
                target: pieceList3
                y: getY(3, 3, pieceList0.height, pieceList1.height,
                        pieceList2.height, pieceList3.height, rowSpacing)
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
                        Math.max(
                            animationDurationMove - animationDurationFast, 0)
                }
                NumberAnimation {
                    target: root
                    property: "opacity"
                    to: 0
                    duration: animationDurationFast
                }
                PropertyAction { property: "y" }
                PropertyAction { target: root; property: "contentY"; value: 0 }
                // Workaround for QTBUG-99739 (Transition does not correctly
                // set value if target changes while transition runs; Qt 6.2)
                ScriptAction {
                    script: {
                        pieceList0.y =
                                getY(gameModel.toPlay, 0, pieceList0.height,
                                     pieceList1.height, pieceList2.height,
                                     pieceList3.height, rowSpacing)
                        pieceList1.y =
                                getY(gameModel.toPlay, 1, pieceList0.height,
                                     pieceList1.height, pieceList2.height,
                                     pieceList3.height, rowSpacing)
                        pieceList2.y =
                                getY(gameModel.toPlay, 2, pieceList0.height,
                                     pieceList1.height, pieceList2.height,
                                     pieceList3.height, rowSpacing)
                        pieceList3.y =
                                getY(gameModel.toPlay, 3, pieceList0.height,
                                     pieceList1.height, pieceList2.height,
                                     pieceList3.height, rowSpacing)
                    }
                }
                NumberAnimation {
                    target: root
                    property: "opacity"
                    to: 1
                    duration: animationDurationFast
                }
            }
    }
}
