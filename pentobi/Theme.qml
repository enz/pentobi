//-----------------------------------------------------------------------------
/** @file pentobi/Theme.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Item {
    /** @name Colors for board and piece elements.
        Each color has several versions to paint raised or sunken borders. The
        first color is the base color, the second a darker version, the third
        a lighter version. The board has three additional colors for painting
        the center section in Callisto. The pieces have a fourth color for
        painting markup. */
    /// @{
    required property list<color> colorBlue
    required property list<color> colorBoard
    required property list<color> colorGreen
    required property list<color> colorOrange
    required property list<color> colorPurple
    required property list<color> colorRed
    required property list<color> colorYellow
    /// @}

    required property color colorBackground
    required property color colorButtonPressed
    required property color colorButtonHovered
    required property color colorButtonBorder
    required property color colorCommentBase
    required property color colorCommentFocus
    required property color colorCommentText
    required property color colorMessageText
    required property color colorMessageBase
    required property color colorSelectedText
    required property color colorSelection
    required property color colorStartingPoint
    required property color colorBoardMarkup
    required property color colorText

    property string imageDir

    function getImage(name) {
        return "themes/" + imageDir + "/" + name + ".png"
    }
}
