//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/colorblind-dark/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import "../colorblind-light" as ColorblindLight
import "../dark" as Dark

Dark.Theme {
    property color colorBlue: "#008f9d"
    property color colorYellow: "#bb7031"
    property color colorRed: "#984326"
    property color colorGreen: "#72a074"
    property color colorPurple: colorBlue
    property color colorOrange: colorRed

    function getImage(name) {
        if (name.startsWith("frame-")
                || name.startsWith("junction-")
                || name.startsWith("linesegment-")
                || name.startsWith("quarter-square-")
                || name.startsWith("square-")
                || name.startsWith("triangle-"))
            return colorblindLight.getImage(name)
        return dark.getImage(name)
    }

    ColorblindLight.Theme { id: colorblindLight }
    Dark.Theme { id: dark }
}
