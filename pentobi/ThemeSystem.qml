//-----------------------------------------------------------------------------
/** @file pentobi/ThemeSystem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Theme {
    property bool _isDark: palette.window.hslLightness < 0.5

    function _contrast(color1, color2) {
        return 0.30 * (color1.r - color2.r) + 0.59 * (color1.g - color2.g)
                + 0.11 * (color1.b - color2.b)
    }

    colorBackground: {
        // If the contrast to yellow is too bad, we use a slightly modified
        // system background color
        var c = _contrast(palette.window, colorYellow)
        if (c > 0 && c < 0.12)
            return Qt.lighter(palette.window, 1.1)
        if (c < 0 && c > -0.12)
            return Qt.darker(palette.window, 1.5)
        return palette.window
    }

    colorBlue: _isDark ? dark.colorBlue : light.colorBlue
    colorBoard: _isDark ? dark.colorBoard : light.colorBoard
    colorGreen: _isDark ? dark.colorGreen : light.colorGreen
    colorOrange: _isDark ? dark.colorOrange : light.colorOrange
    colorPurple: _isDark ? dark.colorPurple : light.colorPurple
    colorRed: _isDark ? dark.colorRed : light.colorRed
    colorYellow: _isDark ? dark.colorYellow : light.colorYellow

    colorButtonPressed: palette.mid
    colorButtonHovered: palette.window
    colorButtonBorder: palette.dark
    colorCommentBase: palette.base
    colorCommentFocus: palette.highlight
    colorCommentText: colorText
    colorMessageText: colorText
    colorMessageBase: palette.base
    colorSelectedText: palette.highlightedText
    colorSelection: palette.highlight
    colorStartingPoint:
        _isDark ? dark.colorStartingPoint : light.colorStartingPoint
    colorBoardMarkup:
        _isDark ? dark.colorBoardMarkup : light.colorBoardMarkup
    colorText: palette.text

    imageDir: _isDark ? dark.imageDir : light.imageDir

    SystemPalette { id: palette }
    ThemeDark { id: dark }
    ThemeLight { id: light }
}
