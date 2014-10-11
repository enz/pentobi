import QtQuick 2.0

Row {
    id: root

    /** Size of the colored points indicating a game color. */
    property real pointSize
    property int toPlay
    property string gameVariant
    property int points0
    property int points1
    property int points2
    property int points3
    property bool hasMoves0
    property bool hasMoves1
    property bool hasMoves2
    property bool hasMoves3

    property int _toPlayShown: toPlay

    onToPlayChanged: {
        if (! transitionsEnabled)
            _toPlayShown = toPlay
        else
            delayShowToPlay.start()
    }

    ScoreElement2 {
        visible: gameVariant == "classic_2" || gameVariant == "trigon_2"
        value: points0 + points2
        isFinal: ! hasMoves0 && ! hasMoves2
        pointSize: root.pointSize
        height: root.height
        width: 6.2 * pointSize
        color1: theme.colorBlue
        color2: theme.colorRed
    }

    ScoreElement2 {
        visible: gameVariant == "classic_2" || gameVariant == "trigon_2"
        value: points1 + points3
        isFinal: ! hasMoves1 && ! hasMoves3
        pointSize: root.pointSize
        height: root.height
        width: 6.2 * pointSize
        color1: theme.colorYellow
        color2: theme.colorGreen
    }

    ScoreElement {
        value: points0
        isFinal: ! hasMoves0
        isToPlay: _toPlayShown == 0
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: theme.colorBlue
    }

    ScoreElement {
        value: points1
        isFinal: ! hasMoves1
        isToPlay: _toPlayShown == 1
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: boardModel.gameVariant == "duo"
               || boardModel.gameVariant == "junior" ?
                   theme.colorGreen : theme.colorYellow
    }

    ScoreElement {
        visible: gameVariant != "duo" && gameVariant != "junior"
        value: points2
        isFinal: ! hasMoves2
        isToPlay: _toPlayShown == 2
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: theme.colorRed
    }

    ScoreElement {
        visible: gameVariant != "duo" && gameVariant != "junior" &&
                 gameVariant != "trigon_3"
        value: points3
        isFinal: ! hasMoves3
        isToPlay: _toPlayShown == 3
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: theme.colorGreen
    }
    Timer {
        id: delayShowToPlay

        interval: 300
        onTriggered: _toPlayShown = toPlay
    }
}
