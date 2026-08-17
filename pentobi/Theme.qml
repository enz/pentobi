//-----------------------------------------------------------------------------
/** @file pentobi/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

/** Provides colors for painting the board and for customizing items.
    Note that many styles have broken styleHints, palette, and/or icon
    coloring. The palette is even broken in dark mode of Basic style
    (QTBUG-148414), so we avoid using it. */
Item {
    property bool colorblind

    property bool isDark: {
        // globalStyle is a context property set in Main.cpp
        if (globalStyle === "Material")
            // styleHints don't work in Material (Qt 6.11) we fixed dark mode
            // in qtquickcontrols2.conf
            return true
        if (globalStyle === "Fusion")
            // styleHints is broken in Fusion (QTBUG-148413)
            return palette.window.hslLightness < 0.5
        return Application.styleHints.colorScheme == Qt.Dark
    }

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
        isDark ? [ "#494949", "#3b3b3b", "#6d6d6d",
                  "#696969", "#5a5a5a", "#797979" ]
               : [ "#aeaeae", "#868686", "#c7c7c7",
                  "#919191", "#7c7c7c", "#a0a0a0" ]
    /// @}

    property color colorStartingPoint: isDark ? "#82777E" : "#767074"
    property color colorBoardMarkup: isDark ? "#ffffff" : "#111111"

    property color colorBackground: isDark ? "#000000" : "#ffffff"
    property color colorMessageBase: colorBackground
    property color colorText: isDark ? "#ffffff" : "#000000"

    // Default link color in dark mode has low contrast (Qt 6.9)
    property color colorLink: isDark ? "lightskyblue" : "blue"
}

