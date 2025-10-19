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

    // Note that this might break in the future because the existence of
    // Menu.font is not documented.
    width: Math.min(font.pixelSize * (isDesktop ? relativeWidth : 23),
                    rootWindow.contentItem.width)
    cascade: isDesktop
    closePolicy: isDesktop ?
                     Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                   : Popup.CloseOnEscape | Popup.CloseOnPressOutside
}
