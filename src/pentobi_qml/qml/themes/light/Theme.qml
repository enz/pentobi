import QtQuick 2.0

QtObject {
    property color backgroundColor: "#B4B2B3"
    property color fontColorScore: "#5A5755"
    property color colorBlue: "#0077D2"
    property color colorYellow: "#EBCD23"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property real toPlayColorLighter: 0.5

    function getImage(name) { return "themes/light/" + name + ".svg" }
}
