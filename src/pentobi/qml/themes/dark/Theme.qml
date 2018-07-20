//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/dark/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.2

QtObject {
    property color busyIndicatorColor: fontColorScore
    property color commentBorderColor: "#5a5756"
    property color commentBorderColorFocus: "#0066ff"
    property color messageTextColor: fontColorPosInfo
    property color messageBackgroundColor: "#333"
    property color backgroundColor: "#131313"
    property color fontColorScore: "#C8C1BE"
    property color fontColorCoordinates: "#A09B99"
    property color fontColorPosInfo: "#C8C1BE"
    property color colorBlue: "#0077D2"
    property color colorYellow: "#e8c616"
    property color colorRed: "#E63E2C"
    property color colorGreen: "#00C000"
    property color colorPurple: "#a12CCF"
    property color colorOrange: "#F09217"
    property color colorStartingPoint: "#82777E"
    property color colorIcon: "#777"
    property color backgroundButtonPressed: Qt.lighter(backgroundColor, 3)
    property color selectionColor: "#96918f"
    property color selectedTextColor: backgroundColor
    property color toolBarTextColor: fontColorPosInfo
    property color analyzeBackgroundColor: backgroundColor
    property color analyzeLineColor: commentBorderColor
    property color analyzeMiddleLineColor: commentBorderColor
    property real pieceListOpacity: 0.94
    property real toPlayColorLighter: 1.7

    function getImage(name) {
        if (name.startsWith("frame-")
                || name.startsWith("junction-")
                || name.startsWith("linesegment-")
                || name.startsWith("piece-manipulator")
                || name.startsWith("quarter-square-")
                || name.startsWith("square-")
                || name.startsWith("triangle-"))
            return "themes/light/" + name + ".svg"
        return "themes/dark/" + name + ".svg"
    }
}
