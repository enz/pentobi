//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/dark/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11

Item {
    property color colorBackground: "#131313"
    property color colorBlue: "#0077D2"
    property color colorButtonPressed: Qt.lighter(colorBackground, 3)
    property color colorCommentBorder: "#5a5756"
    property color colorCommentFocus: colorText
    property color colorGreen: "#00C000"
    property color colorMessageText: colorText
    property color colorMessageBase: "#333"
    property color colorOrange: "#F09217"
    property color colorPurple: "#a12CCF"
    property color colorRed: "#E63E2C"
    property color colorSelectedText: colorBackground
    property color colorSelection: "#96918f"
    property color colorStartingPoint: "#82777E"
    property color colorText: "#C8C1BE"
    property color colorYellow: "#e8c616"

    property real opacityCoordinates: 0.5
    property real opacityScore: 0.7
    property real opacitySubduedText: 0.5
    property real pieceListOpacity: 0.94
    property real toPlayColorLighter: 1.7

    function getImage(name) {
        if (name.startsWith("frame-")
                || name.startsWith("junction-")
                || name.startsWith("linesegment-")
                || name.startsWith("piece-manipulator")
                || name.startsWith("quarter-square-")
                || name.startsWith("square-")
                || name.startsWith("triangle-")
                || (name.startsWith("pentobi-") && name !== "pentobi-rated-game")
                || name === "menu")
            return "themes/light/" + name + ".svg"
        return "themes/dark/" + name + ".svg"
    }
}
