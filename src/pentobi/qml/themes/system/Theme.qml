//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/system/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import "../dark" as Dark
import "../light" as Light

// See themes/light/Theme.qml for comments
Item {
    property color colorBackground: {
        // If the contrast to yellow is too bad, we use a slightly modified
        // system background color
        var c = _contrast(palette.window, colorYellow)
        if (c > 0 && c < 0.12)
            return Qt.lighter(palette.window, 1.1)
        if (c < 0 && c > -0.12)
            return Qt.darker(palette.window, 1.5)
        return palette.window
    }

    property var colorBlue: _isDark ? dark.colorBlue : light.colorBlue
    property var colorBoard: _isDark ? dark.colorBoard : light.colorBoard
    property var colorGreen: _isDark ? dark.colorGreen : light.colorGreen
    property var colorOrange: _isDark ? dark.colorOrange : light.colorOrange
    property var colorPurple: _isDark ? dark.colorPurple : light.colorPurple
    property var colorRed: _isDark ? dark.colorRed : light.colorRed
    property var colorYellow: _isDark ? dark.colorYellow : light.colorYellow

    property color colorButtonPressed: palette.mid
    property color colorButtonHovered: palette.window
    property color colorButtonBorder: palette.dark
    property color colorCommentBase: palette.base
    property color colorCommentBorder: palette.mid
    property color colorCommentFocus: palette.dark
    property color colorCommentText: colorText
    property color colorMessageText: colorText
    property color colorMessageBase: palette.base
    property color colorSelectedText: palette.highlightedText
    property color colorSelection: palette.highlight
    property color colorStartingPoint:
        _isDark ? dark.colorStartingPoint : light.colorStartingPoint
    property color colorText: palette.text

    property bool _isDark: palette.window.hslLightness < 0.5

    function getImage(name) {
        return _isDark ? dark.getImage(name) : light.getImage(name)
    }

    function _contrast(color1, color2) {
        return 0.30 * (color1.r - color2.r) + 0.59 * (color1.g - color2.g)
                + 0.11 * (color1.b - color2.b)
    }

    SystemPalette { id: palette }
    Dark.Theme { id: dark }
    Light.Theme { id: light }
}
