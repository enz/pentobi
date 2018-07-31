//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/system/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import "../dark" as Dark
import "../light" as Light

Item {
    property color colorBackground: palette.window
    property color colorBlue: _isDark ? dark.colorBlue : light.colorBlue
    property color colorButtonPressed: palette.mid
    property color colorButtonHovered: palette.light
    property color colorButtonBorder: palette.shadow
    property color colorCommentBorder: palette.mid
    property color colorCommentFocus: palette.highlight
    property color colorGreen: _isDark ? dark.colorGreen : light.colorGreen
    property color colorMessageText: colorText
    property color colorMessageBase: palette.base
    property color colorOrange: _isDark ? dark.colorOrange : light.colorOrange
    property color colorPieceManipulatorBase:
        _isDark ? dark.colorPieceManipulatorBase
                : light.colorPieceManipulatorBase
    property color colorPieceManipulatorBorder:
        _isDark ? dark.colorPieceManipulatorBorder
                : light.colorPieceManipulatorBorder
    property color colorPieceManipulatorLegal:
        _isDark ? dark.colorPieceManipulatorLegal
                : light.colorPieceManipulatorLegal
    property color colorPurple: _isDark ? dark.colorPurple : light.colorPurple
    property color colorRed: _isDark ? dark.colorRed : light.colorRed
    property color colorSelectedText: palette.highlightedText
    property color colorSelection: palette.highlight
    property color colorStartingPoint:
        _isDark ? dark.colorStartingPoint : light.colorStartingPoint
    property color colorText: palette.text
    property color colorYellow: _isDark ? dark.colorYellow : light.colorYellow
    property real opacitySubduedText:
        0.55 - 0.2 * theme.colorBackground.hslLightness

    property bool _isDark: colorBackground.hslLightness < 0.5

    function getImage(name) {
        return _isDark ? dark.getImage(name) : light.getImage(name)
    }

    SystemPalette { id: palette }
    Dark.Theme { id: dark }
    Light.Theme { id: light }
}
