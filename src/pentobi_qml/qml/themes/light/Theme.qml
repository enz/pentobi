//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/themes/light/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

QtObject {
    property color backgroundGradientColor1: "#D2CFD1"
    property color backgroundGradientColor2: "#CDCACC"
    property color fontColorScore: "#5A5755"
    property color fontColorMessage: "#F5F1EF"
    property color backgroundColorMessage: "#64605F"
    property color colorBlue: "#0077D2"
    property color colorYellow: "#EBCD23"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property real toPlayColorLighter: 0.5

    function getImage(name) { return "themes/light/" + name + ".svg" }
}
