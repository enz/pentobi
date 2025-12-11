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

    width: Math.min(font.pixelSize * (isDesktop ? relativeWidth : 23),
                    rootWindow.contentItem.width)
    cascade: isDesktop
    closePolicy: isDesktop ?
                     Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                   : Popup.CloseOnEscape | Popup.CloseOnPressOutside
    // Material style highlights first item in menu (see also QTBUG-69541)
    onAboutToShow:
        if (globalStyle === "Material" && ! isDesktop)
            currentIndex = -1
}
