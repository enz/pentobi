//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/colorblind-light/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.11
import "../light" as Light

Light.Theme {
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
        return light.getImage(name)
    }

    Light.Theme { id: light }
}
