//-----------------------------------------------------------------------------
/** @file pentobi/qml/PentobiMenu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2

Menu {
    // Width in font-size units
    property real relativeWidth: 23

    width: Math.min(font.pixelSize * (isDesktop ? relativeWidth : 23),
                    rootWindow.contentItem.width)
    cascade: isDesktop
    closePolicy: isDesktop ?
                     Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                   : Popup.CloseOnEscape | Popup.CloseOnPressOutside
    delegate: PentobiMenuItem { }
    background: Rectangle {
        // Note that MenuItem in Qt 5.11 does neither fully use the system
        // palette, nor make its actually used colors available.
        color: isDesktop ? palette.window : palette.base
        border.color: palette.mid
    }
    // Workaround for QTBUG-69541 (Opened Menu highlights last used item on Android)
    onOpened: if (isAndroid) currentIndex = -1
}
