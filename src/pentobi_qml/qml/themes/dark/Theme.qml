import QtQuick 2.0

QtObject {
    property color backgroundColor: "#131313"
    property color fontColorScore: "#C8C1BE"
    property color fontColorPosInfo: "#C8C1BE"
    property color colorBlue: "#0077D2"
    property color colorYellow: "#EBCD23"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property color colorStartingPoint: "#82777E"
    property real pieceListOpacity: 0.94
    property real toPlayColorLighter: 1.8

    function getImage(name) {
        if (name.lastIndexOf("frame-", 0) === 0
                || name.lastIndexOf("junction-", 0) === 0
                || name.lastIndexOf("linesegment-", 0) === 0
                || name.lastIndexOf("piece-manipulator", 0) === 0
                || name.lastIndexOf("square-", 0) === 0
                || name.lastIndexOf("triangle-", 0) === 0)
            return "themes/light/" + name + ".svg"
        return "themes/dark/" + name + ".svg"
    }
}
