//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/dark/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import "../light" as Light

// See themes/light/Theme.qml for comments
Light.Theme {
    property var colorBoard: [ "#494347", "#3b3639", "#6d686b",
                               "#696267", "#5a5458", "#797276" ]

    property color colorBackground: "#131313"
    property color colorButtonPressed: Qt.lighter(colorBackground, 4)
    property color colorButtonHovered: Qt.lighter(colorBackground, 2)
    property color colorButtonBorder: Qt.lighter(colorBackground, 5)
    property color colorCommentBase: "#1e2028"
    property color colorCommentBorder: "#5a5756"
    property color colorCommentFocus: colorText
    property color colorMessageText: colorText
    property color colorMessageBase: "#333"
    property color colorPieceManipulator: "#333333"
    property color colorPieceManipulatorLegal: "#a8a8a8"
    property color colorSelectedText: colorBackground
    property color colorSelection: "#96918f"
    property color colorStartingPoint: "#82777E"
    property color colorText: "#C8C1BE"
    property real opacitySubduedText: 0.59

    function getImage(name) {
        if (name.startsWith("frame-")
                || name.startsWith("junction-")
                || name.startsWith("linesegment-")
                || name.startsWith("quarter-square-")
                || name.startsWith("triangle-")
                || (name.startsWith("pentobi-") && name !== "pentobi-rated-game")
                || name === "menu")
            return light.getImage(name)
        return "themes/dark/" + name + ".svg"
    }

    Light.Theme { id: light }
}
