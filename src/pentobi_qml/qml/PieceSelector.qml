import QtQuick 2.0

Flickable {
    id: root

    property string gameVariant
    property int toPlay
    property var pieces0
    property var pieces1
    property var pieces2
    property var pieces3
    property int nuColors
    property int columns
    property real spacingPieceLists
    property bool transitionsEnabled

    signal piecePicked(var piece)

    contentHeight: pieceList0.height + pieceList1.height + pieceList2.height
                   + pieceList3.height + nuColors * 0.5 * spacingPieceLists
    flickableDirection: Flickable.VerticalFlick
    clip: true

    PieceList {
        id: pieceList0

        width: root.width
        columns: root.columns
        pieces: pieces0
        onPiecePicked: root.piecePicked(piece)
    }
    PieceList {
        id: pieceList1

        width: root.width
        columns: root.columns
        pieces: pieces1
        onPiecePicked: root.piecePicked(piece)
    }
    PieceList {
        id: pieceList2

        width: root.width
        columns: root.columns
        pieces: pieces2
        onPiecePicked: root.piecePicked(piece)
    }
    PieceList {
        id: pieceList3

        width: root.width
        columns: root.columns
        pieces: pieces3
        onPiecePicked: root.piecePicked(piece)
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
                y: 0.25 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList1
                y: gameVariant != "classic_2" && gameVariant != "trigon_2"
                   && gameVariant != "nexos_2" ?
                       pieceList0.height + 0.75 * spacingPieceLists :
                       pieceList0.height + pieceList2.height + 1.25 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList2
                y: gameVariant != "classic_2" && gameVariant != "trigon_2"
                   && gameVariant != "nexos_2" ?
                       pieceList0.height + pieceList1.height  + 1.25 * spacingPieceLists :
                       pieceList0.height + 0.75 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList3
                y: pieceList0.height + pieceList1.height + pieceList2.height + 1.75 * spacingPieceLists
            }
        },
        State {
            name: "toPlay1"
            when: toPlay === 1

            PropertyChanges {
                target: pieceList1
                y: 0.25 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList2
                y: gameVariant != "classic_2" && gameVariant != "trigon_2"
                   && gameVariant != "nexos_2" ?
                       pieceList1.height + 0.75 * spacingPieceLists :
                       pieceList1.height + pieceList3.height + 1.25 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList3
                y: gameVariant != "classic_2" && gameVariant != "trigon_2"
                   && gameVariant != "nexos_2" ?
                       pieceList1.height + pieceList2.height + 1.25 * spacingPieceLists :
                       pieceList1.height + 0.75 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList0
                y: pieceList1.height + pieceList2.height + pieceList3.height + 1.75 * spacingPieceLists
            }
        },
        State {
            name: "toPlay2"
            when: toPlay === 2

            PropertyChanges {
                target: pieceList2
                y: 0.25 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList3
                y: gameVariant != "classic_2" && gameVariant != "trigon_2"
                   && gameVariant != "nexos_2" ?
                       pieceList2.height + 0.75 * spacingPieceLists :
                       pieceList2.height + pieceList0.height + 1.25 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList0
                y: gameVariant != "classic_2" && gameVariant != "trigon_2"
                   && gameVariant != "nexos_2" ?
                       pieceList2.height + pieceList3.height + 1.25 * spacingPieceLists :
                       pieceList2.height + 0.75 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList1
                y: pieceList2.height + pieceList3.height + pieceList0.height + 1.75 * spacingPieceLists
            }
        },
        State {
            name: "toPlay3"
            when: toPlay === 3

            PropertyChanges {
                target: pieceList3
                y: 0.25 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList0
                y: gameVariant != "classic_2" && gameVariant != "trigon_2"
                   && gameVariant != "nexos_2" ?
                       pieceList3.height + 0.75 * spacingPieceLists :
                       pieceList3.height + pieceList1.height + 1.25 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList1
                y: gameVariant != "classic_2" && gameVariant != "trigon_2"
                   && gameVariant != "nexos_2" ?
                       pieceList3.height + pieceList0.height + 1.25 * spacingPieceLists :
                       pieceList3.height + 0.75 * spacingPieceLists
            }
            PropertyChanges {
                target: pieceList2
                y: pieceList3.height + pieceList0.height + pieceList1.height + 1.75 * spacingPieceLists
            }
        }
    ]
    transitions:
        Transition {
        enabled: transitionsEnabled

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
            PauseAnimation { duration: 200 }
            NumberAnimation {
                target: root; property: "opacity"; to: 0; duration: 100
            }
            PropertyAction { target: pieceList0; property: "y" }
            PropertyAction { target: pieceList1; property: "y" }
            PropertyAction { target: pieceList2; property: "y" }
            PropertyAction { target: pieceList3; property: "y" }
            PropertyAction { target: root; property: "contentY"; value: 0 }
            NumberAnimation {
                target: root; property: "opacity"; to: 1; duration: 100
            }
        }
    }
}
