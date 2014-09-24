//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/themes/dark/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

QtObject {
    property color backgroundGradientColor1: "#827D7B"
    property color backgroundGradientColor2: "#64605F"
    property color fontColorScore: "#C8C1BE"
    property color fontColorMessage: "#E6DFDC"
    property color backgroundColorMessage: "#282626"
    property color colorBlue: "#0073CF"
    property color colorYellow: "#EBCD23"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property real toPlayColorLighter: 1.6

    function getImage(name) {
        switch (name) {
        case "board-classic":
        case "board-duo":
        case "board-trigon-3":
        case "board-trigon":
        case "flick-left":
        case "flick-right":
            return "themes/dark/" + name + ".svg"
        default:
            return "themes/light/" + name + ".svg"
        }
    }
}
