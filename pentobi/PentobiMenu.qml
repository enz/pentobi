//-----------------------------------------------------------------------------
/** @file pentobi/PentobiMenu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

Menu {
    // Width in font-size units
    property real relativeWidth: 23

    width: Math.min(font.pixelSize * (isMobile ? 23 : relativeWidth),
                    rootWindow.contentItem.width)
    closePolicy: ! isMobile ?
                     Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                   : Popup.CloseOnEscape | Popup.CloseOnPressOutside
    // See PentobiMenuItem.focusPolicy
    focus: ! isMobile
    onAboutToShow: currentIndex = -1
}
