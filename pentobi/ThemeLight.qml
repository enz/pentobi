//-----------------------------------------------------------------------------
/** @file pentobi/ThemeLight.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Theme {
    colorBlue: [ "#0073cf", "#004881", "#1499ff", "#ffffff" ]
    colorBoard: [ "#aea7ac", "#868084", "#c7bfc5",
                  "#918b8f", "#7c777b", "#a09a9f"]
    colorGreen: [ "#00c000", "#007800", "#00fa00", "#333333" ]
    colorOrange: [ "#f09217", "#9d5e0b", "#ffbb67", "#333333" ]
    colorPurple: [ "#a12ccf", "#6d2787", "#be70dc", "#ffffff" ]
    colorRed: [ "#e63e2c", "#90261b", "#ff655a", "#ffffff" ]
    colorYellow: [ "#f5c320", "#aa8516", "#ffdb58", "#333333" ]

    colorBackground: "#e8e8e8"
    colorButtonPressed: Qt.darker(colorBackground, 1.1)
    colorButtonHovered: Qt.lighter(colorBackground, 3)
    colorButtonBorder: Qt.darker(colorBackground, 2)
    colorCommentBase: "#ffffff"
    colorCommentFocus: "#4799cc"
    colorCommentText: colorText
    colorMessageText: "black"
    colorMessageBase: "#e0e0e0"
    colorSelectedText: colorBackground
    colorSelection: "#4799cc"
    colorStartingPoint: "#767074"
    colorBoardMarkup: "#111111"
    colorText: "#111111"

    imageDir: "light"
}
