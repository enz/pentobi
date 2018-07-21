//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/light/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.2

QtObject {
    property color colorBackground: "#dcdad6"
    property color colorBlue: "#0077D2"
    property color colorButtonPressed: Qt.darker(colorBackground, 1.1)
    property color colorCommentFocus: "#0066ff"
    property color colorCoordinates: "#64615E"
    property color colorGreen: "#00C000"
    property color colorMessageText: "black"
    property color colorMessageBase: "#cac9c9"
    property color colorOrange: "#F09217"
    property color colorPurple: "#a12CCF"
    property color colorRed: "#E63E2C"
    property color colorScore: "#666"
    property color colorSelectedText: colorBackground
    property color colorSelection: "#645f5c"
    property color colorStartingPoint: "#767074"
    property color colorSubduedText: "#848484"
    property color colorText: "#282625"
    property color colorYellow: "#e8c616"

    property real pieceListOpacity: 1
    property real toPlayColorLighter: 0.5

    function getImage(name) { return "themes/light/" + name + ".svg" }
}
