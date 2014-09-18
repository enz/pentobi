//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/ScoreDisplay.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import pentobi 1.0

Row {
    id: root

    property real elementWidth
    property real elementWidth2
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
        height: root.height
        width: elementWidth2
        color1: "#0073CF"
        color2: "#E63E2C"
    }

    ScoreElement2 {
        visible: gameVariant == "classic_2" || gameVariant == "trigon_2"
        value: points1 + points3
        isFinal: ! hasMoves1 && ! hasMoves3
        height: root.height
        width: elementWidth2
        color1: "#EBCD23"
        color2: "#00C000"
    }

    ScoreElement {
        value: points0
        isFinal: ! hasMoves0
        height: root.height
        width: elementWidth
        color: boardModel.gameVariant == "duo" ? "#0073CF" : "#0073CF"
    }

    ScoreElement {
        value: points1
        isFinal: ! hasMoves1
        height: root.height
        width: elementWidth
        color: boardModel.gameVariant == "duo" ? "#00C000" : "#EBCD23"
    }

    ScoreElement {
        visible: gameVariant != "duo"
        value: points2
        isFinal: ! hasMoves2
        height: root.height
        width: elementWidth
        color: "#E63E2C"
    }

    ScoreElement {
        visible: gameVariant != "duo"
        value: points3
        isFinal: ! hasMoves3
        height: root.height
        width: elementWidth
        color: "#00C000"
    }
}
