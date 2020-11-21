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
    property color colorButtonPressed: Qt.lighter(colorBackground, 5)
    property color colorButtonHovered: Qt.lighter(colorBackground, 2)
    property color colorButtonBorder: Qt.lighter(colorBackground, 5)
    property color colorCommentBase: "#1e2028"
    property color colorCommentBorder: "#5a5756"
    property color colorCommentFocus: "#4799cc"
    property color colorCommentText: "#C8C1BE"
    property color colorMessageText: "#C8C1BE"
    property color colorMessageBase: "#333333"
    property color colorSelectedText: colorBackground
    property color colorSelection: "#4799cc"
    property color colorStartingPoint: "#82777E"
    property color colorText: "#e6d5e1"

    function getImage(name) { return "themes/dark/" + name + ".png" }

    Light.Theme { id: light }
}
