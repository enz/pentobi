import QtQuick 2.0

QtObject {
    property color busyIndicatorColor: fontColorScore
    property color commentBorderColor: "#968f8b"
    property color messageTextColor: "black"
    property color messageBackgroundColor: "#cac9c9"
    property color backgroundColor: "#dcdbda"
    property color fontColorScore: "#5A5755"
    property color fontColorCoordinates: "#64615E"
    property color fontColorPosInfo: "#282625"
    property color colorBlue: "#0077D2"
    property color colorYellow: "#EBCD23"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property color colorPurple: "#a12CCF"
    property color colorOrange: "#F09217"
    property color colorStartingPoint: "#767074"
    property color backgroundButtonPressed: Qt.lighter(backgroundColor)
    property color selectionColor: "steelblue"
    property color selectedTextColor: "#EEE"
    property color toolBarTextColor: "black"
    property color analyzeBackgroundColor: "white"
    property color analyzeLineColor: "black"
    property color analyzeMiddleLineColor: "grey"
    property real pieceListOpacity: 1
    property real toPlayColorLighter: 0.5

    function getImage(name) { return "themes/light/" + name + ".svg" }
}
