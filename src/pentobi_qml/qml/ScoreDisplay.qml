import QtQuick 2.0

Row {
    id: root

    property real pointSize
    property int toPlay
    property int altPlayer
    property string gameVariant
    property int points0
    property int points1
    property int points2
    property int points3
    property int bonus0
    property int bonus1
    property int bonus2
    property int bonus3
    property bool hasMoves0
    property bool hasMoves1
    property bool hasMoves2
    property bool hasMoves3

    ScoreElement2 {
        visible: gameVariant == "classic_2" || gameVariant == "trigon_2"
                 || gameVariant == "nexos_2"
        value: points0 + points2
        isFinal: ! hasMoves0 && ! hasMoves2
        pointSize: root.pointSize
        height: root.height
        width: 5.9 * pointSize
        color1: theme.colorBlue
        color2: theme.colorRed
    }
    ScoreElement2 {
        visible: gameVariant == "classic_2" || gameVariant == "trigon_2"
                 || gameVariant == "nexos_2"
        value: points1 + points3
        isFinal: ! hasMoves1 && ! hasMoves3
        pointSize: root.pointSize
        height: root.height
        width: 5.9 * pointSize
        color1: theme.colorYellow
        color2: theme.colorGreen
    }
    ScoreElement {
        value: points0
        bonus: bonus0
        isFinal: ! hasMoves0
        isToPlay: toPlay == 0
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: theme.colorBlue
    }
    ScoreElement {
        value: points1
        bonus: bonus1
        isFinal: ! hasMoves1
        isToPlay: toPlay == 1
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: gameModel.gameVariant == "duo"
                   || gameModel.gameVariant == "junior" ?
                   theme.colorGreen : theme.colorYellow
    }
    ScoreElement {
        visible: gameVariant != "duo" && gameVariant != "junior"
        value: points2
        bonus: bonus2
        isFinal: ! hasMoves2
        isToPlay: toPlay == 2
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: theme.colorRed
    }
    ScoreElement {
        visible: gameVariant != "duo" && gameVariant != "junior" &&
                 gameVariant != "trigon_3" && gameVariant != "classic_3"
        value: points3
        bonus: bonus3
        isFinal: ! hasMoves3
        isToPlay: toPlay == 3
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: theme.colorGreen
    }
    ScoreElement2 {
        visible: gameVariant == "classic_3"
        value: points3
        isAltColor: true
        isToPlay: toPlay == 3
        isFinal: ! hasMoves3
        pointSize: root.pointSize
        height: root.height
        width: 5.9 * pointSize
        color1: theme.colorGreen
        color2:
            switch (altPlayer) {
            case 0: return theme.colorBlue
            case 1: return theme.colorYellow
            case 2: return theme.colorRed
            }
    }
}
