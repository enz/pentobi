//-----------------------------------------------------------------------------
/** @file pentobi/ThemeDark.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

ThemeLight {
    colorBoard: [ "#494347", "#3b3639", "#6d686b",
                  "#696267", "#5a5458", "#797276" ]

    colorBackground: "#131313"
    colorButtonPressed: Qt.lighter(colorBackground, 5)
    colorButtonHovered: Qt.lighter(colorBackground, 2)
    colorButtonBorder: Qt.lighter(colorBackground, 5)
    colorCommentBase: "#1e2028"
    colorCommentFocus: "#4799cc"
    colorCommentText: "#C8C1BE"
    colorMessageText: "#C8C1BE"
    colorMessageBase: "#333333"
    colorSelectedText: colorBackground
    colorSelection: "#4799cc"
    colorStartingPoint: "#82777E"
    colorBoardMarkup: "#ffffff"
    colorText: "#e6d5e1"

    imageDir: "dark"

    ThemeLight { id: light }
}
