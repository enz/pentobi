//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/themes/light/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

QtObject {
    property color backgroundGradientColor1: "#BEBBBB"
    property color backgroundGradientColor2: "#8C8A89"
    property color fontColorScore: "#5A5755"
    property color fontColorMessage: "#F5F1EF"
    property color backgroundColorMessage: "#64605F"
    property color colorBlue: "#0073CF"
    property color colorYellow: "#EBCD23"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property real toPlayColorLighter: 0.5

    function getImage(name) { return "themes/light/" + name + ".svg" }
}
