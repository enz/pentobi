//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/colorblind-light/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.11
import "../light" as Light

// See themes/light/Theme.qml for comments
Light.Theme {
    property var colorBlue: [ "#008f9d", "#006069", "#00bcce", "#ffffff" ]
    property var colorGreen: [ "#72a074", "#4e7450", "#9cbc9e", "#ffffff" ]
    property var colorOrange: colorRed
    property var colorPurple: colorBlue
    property var colorRed: [ "#984326", "#692e19", "#ca5a30", "#ffffff" ]
    property var colorYellow: [ "#bb7031", "#8c5525", "#d28b4f", "#ffffff" ]

    function getImage(name) {
        if (name === "square-purple")
            return "themes/colorblind-light/square-blue.svg"
        if (name === "square-orange")
            return "themes/colorblind-light/square-yellow.svg"
        if (name.startsWith("frame-")
                || name.startsWith("junction-")
                || name.startsWith("linesegment-")
                || name.startsWith("quarter-square-")
                || name.startsWith("triangle-"))
            return "themes/colorblind-light/" + name + ".svg"
        return light.getImage(name)
    }

    Light.Theme { id: light }
}
