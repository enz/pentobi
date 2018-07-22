//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/colorblind-dark/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.11
import "../dark" as Dark

Dark.Theme {
    property color colorBlue: "#008f9d"
    property color colorYellow: "#bb7031"
    property color colorRed: "#984326"
    property color colorGreen: "#72a074"
    property color colorPurple: colorBlue
    property color colorOrange: colorRed
    function getImage(name) {
        if (name === "square-purple")
            return "themes/colorblind-light/square-blue.svg"
        if (name === "square-orange")
            return "themes/colorblind-light/square-yellow.svg"
        if (name.startsWith("frame-")
                || name.startsWith("junction-")
                || name.startsWith("linesegment-")
                || name.startsWith("quarter-square-")
                || name.startsWith("square-")
                || name.startsWith("triangle-"))
            return "themes/colorblind-light/" + name + ".svg"
        if (name.startsWith("piece-manipulator"))
            return "themes/light/" + name + ".svg"
        return "themes/dark/" + name + ".svg"
    }
}
