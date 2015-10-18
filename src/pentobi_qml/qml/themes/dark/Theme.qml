import QtQuick 2.0

QtObject {
    property color backgroundColor: "#131313"
    property color fontColorScore: "#C8C1BE"
    property color fontColorMessage: "#E6DFDC"
    property color backgroundColorMessage: "#282626"
    property color colorBlue: "#0077D2"
    property color colorYellow: "#EBCD23"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property real toPlayColorLighter: 1.6

    function getImage(name) { return "themes/dark/" + name + ".svg" }
}
