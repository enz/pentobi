//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/ScoreDisplay.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

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

    ScoreElement2 {
        visible: gameVariant == "classic_2" || gameVariant == "trigon_2"
        value: points0 + points2
        isFinal: ! hasMoves0 && ! hasMoves2
        pointSize: root.pointSize
        height: root.height
        width: 6.2 * pointSize
        color1: "#0073CF"
        color2: "#E63E2C"
    }

    ScoreElement2 {
        visible: gameVariant == "classic_2" || gameVariant == "trigon_2"
        value: points1 + points3
        isFinal: ! hasMoves1 && ! hasMoves3
        pointSize: root.pointSize
        height: root.height
        width: 6.2 * pointSize
        color1: "#EBCD23"
        color2: "#00C000"
    }

    ScoreElement {
        value: points0
        isFinal: ! hasMoves0
        isToPlay: toPlay == 0
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: boardModel.gameVariant == "duo" ||
               boardModel.gameVariant == "junior" ? "#0073CF" : "#0073CF"
    }

    ScoreElement {
        value: points1
        isFinal: ! hasMoves1
        isToPlay: toPlay == 1
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: boardModel.gameVariant == "duo"
               || boardModel.gameVariant == "junior" ? "#00C000" : "#EBCD23"
    }

    ScoreElement {
        visible: gameVariant != "duo" && gameVariant != "junior"
        value: points2
        isFinal: ! hasMoves2
        isToPlay: toPlay == 2
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: "#E63E2C"
    }

    ScoreElement {
        visible: gameVariant != "duo" && gameVariant != "junior" &&
                 gameVariant != "trigon_3"
        value: points3
        isFinal: ! hasMoves3
        isToPlay: toPlay == 3
        pointSize: root.pointSize
        height: root.height
        width: 5 * pointSize
        color: "#00C000"
    }
}
