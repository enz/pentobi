//-----------------------------------------------------------------------------
/** @file pentobi/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

/** Provides colors for painting the board and customizing items according
    to the system palette.
    Note that Material style doesn't implement the system palette yet (Qt 6.9),
    so we hardcode some colors and support only Material.Dark. */
Item {
    property bool colorblind

    // globalStyle is a contxt property set in Main.cpp
    property bool isMaterial: globalStyle === "Material"

    // Don't use Applyication.styleHints.colorScheme. On some platforms/styles,
    // it is Qt.Dark in dark mode, even if the style doesn't support dark mode
    // and we better stay in light mode than use a broken dark mode.
    property bool isDark: isMaterial || palette.window.hslLightness < 0.5

    /** @name Colors for board and piece elements.
        Each color has several versions to paint raised or sunken borders. The
        first color is the base color, the second a darker version, the third
        a lighter version. The board has three additional colors for painting
        the center section in Callisto. The pieces have a fourth color for
        painting markup. */
    /// @{
    property list<color> colorBlue:
        colorblind ? [ "#008f9d", "#006069", "#00bcce", "#ffffff" ]
                   : [ "#0073cf", "#004881", "#1499ff", "#ffffff" ]
    property list<color> colorGreen:
        colorblind ? [ "#72a074", "#4e7450", "#9cbc9e", "#ffffff" ]
                   : [ "#00c000", "#007800", "#00fa00", "#333333" ]
    property list<color> colorRed:
        colorblind ? [ "#984326", "#692e19", "#ca5a30", "#ffffff" ]
                   : [ "#e63e2c", "#90261b", "#ff655a", "#ffffff" ]
    property list<color> colorYellow:
        colorblind ? [ "#bb7031", "#8c5525", "#d28b4f", "#ffffff" ]
                   : [ "#f5c320", "#aa8516", "#ffdb58", "#333333" ]
    property list<color> colorOrange:
        colorblind ? [ "#984326", "#692e19", "#ca5a30", "#ffffff" ]
                   : [ "#f09217", "#9d5e0b", "#ffbb67", "#333333" ]
    property list<color> colorPurple:
        colorblind ? [ "#008f9d", "#006069", "#00bcce", "#ffffff" ]
                   : [ "#a12ccf", "#6d2787", "#be70dc", "#ffffff" ]

    property list<color> colorBoard:
        isDark ? [ "#494347", "#3b3639", "#6d686b",
                  "#696267", "#5a5458", "#797276" ]
               : [ "#aea7ac", "#868084", "#c7bfc5",
                  "#918b8f", "#7c777b", "#a09a9f"]
    /// @}

    property color colorStartingPoint: isDark ? "#82777E" : "#767074"
    property color colorBoardMarkup: isDark ? "#ffffff" : "#111111"

    // Should be the dame as used in dark/light SVG icon sources
    property color colorIcon: isDark ? "#91888f" : "#7b7b7b"

    property color colorBackground: isMaterial ? "#1c1b1f" : palette.window
    property color colorButtonHovered: Qt.alpha(colorIcon, 0.2)
    property color colorButtonPressed: Qt.alpha(colorIcon, 0.4)
    property color colorMessageBase: isMaterial ? "#333333" : palette.base
    property color colorText: isMaterial ? "#e6d5e1" : palette.text

    // Default link color in dark mode has low contrast (Qt 6.9)
    property color colorLink: isDark ? "lightskyblue" : "blue"

    function getImage(name) {
        if (isDark)
            return "themes/dark/" + name + ".png"
        else
            return "themes/light/" + name + ".png"
    }
}

