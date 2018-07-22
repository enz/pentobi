//-----------------------------------------------------------------------------
/** @file pentobi/qml/themes/system/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import "../light" as Light

Light.Theme {
    SystemPalette { id: palette }

    property color colorBackground: palette.window
    property color colorButtonPressed: palette.dark
    property color colorCommentBorder: palette.mid
    property color colorCommentFocus: palette.highlight
    property color colorMessageText: colorText
    property color colorMessageBase: palette.base
    property color colorSelectedText: palette.highlightedText
    property color colorSelection: palette.highlight
    property color colorText: palette.text

    property real lighterToPlay: 1.5 - colorBackground.hslLightness
    property real opacityPieceList: colorBackground.hslLightness < 0.5 ? 0.94 : 1
    property real opacitySubduedText: 0.4

    function getImage(name) {
        if (name === "pentobi-rated-game" && colorText.hslLightness > 0.5)
            return "themes/dark/" + name + ".svg"
        return "themes/light/" + name + ".svg"
    }
}
