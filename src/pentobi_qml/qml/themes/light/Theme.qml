import QtQuick 2.0

QtObject {
    property color backgroundColor: "#E6E5E5"
    property color fontColorScore: "#5A5755"
    property color fontColorPosInfo: "#282625"
    property color colorBlue: "#0077D2"
    property color colorYellow: "#EBCD23"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property color colorStartingPoint: "#767074"
    property real toPlayColorLighter: 0.5

    function getImage(name) { return "themes/light/" + name + ".svg" }
}
