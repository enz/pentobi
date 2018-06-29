import QtQuick 2.0

QtObject {
    property color busyIndicatorColor: fontColorScore
    property color commentBorderColor: "#64615f"
    property color messageTextColor: fontColorPosInfo
    property color messageBackgroundColor: "#333"
    property color backgroundColor: "#131313"
    property color fontColorScore: "#C8C1BE"
    property color fontColorCoordinates: "#A09B99"
    property color fontColorPosInfo: "#C8C1BE"
    property color colorBlue: "#008f9d"
    property color colorYellow: "#bb7031"
    property color colorRed: "#984326"
    property color colorGreen: "#72a074"
    property color colorPurple: "#008f9d"
    property color colorOrange: "#bb7031"
    property color colorStartingPoint: "#82777E"
    property color backgroundButtonPressed: Qt.lighter(backgroundColor, 3)
    property color selectionColor: "steelblue"
    property color selectedTextColor: "#EEE"
    property color toolBarTextColor: fontColorPosInfo
    property color analyzeBackgroundColor: "#222"
    property color analyzeLineColor: "#888"
    property color analyzeMiddleLineColor: "#555"
    property real pieceListOpacity: 0.94
    property real toPlayColorLighter: 1.7

    function getImage(name) {
        if (name === "square-purple")
            return "themes/colorblind/square-blue.svg"
        if (name === "square-orange")
            return "themes/colorblind/square-yellow.svg"
        if (name.startsWith("frame-")
                || name.startsWith("junction-")
                || name.startsWith("linesegment-")
                || name.startsWith("quarter-square-")
                || name.startsWith("square-")
                || name.startsWith("triangle-"))
            return "themes/colorblind/" + name + ".svg"
        if (name.startsWith("piece-manipulator"))
            return "themes/light/" + name + ".svg"
        return "themes/dark/" + name + ".svg"
    }
}
