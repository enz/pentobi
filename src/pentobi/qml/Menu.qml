//-----------------------------------------------------------------------------
/** @file pentobi/qml/Menu.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import "Controls.js" as PentobiControls
import "." as Pentobi

Menu {
    function addMnemonic(text, mnemonic) { return PentobiControls.addMnemonic(text, mnemonic) }

    // width should be the same as in Pentobi.MenuItem
    width: Math.min(font.pixelSize * (isDesktop ? 23 : 18), rootWindow.width)
    cascade: isDesktop
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    delegate: Pentobi.MenuItem { }
    background: Rectangle {
        // Note that MenuItem in Qt 5.11 does neither fully use the system
        // palette, nor make its actually used colors available in its own
        // palette.
        color: isDesktop ? palette.window : palette.base
        border.color: palette.dark
    }
    // Workaround for QTBUG-69541 (Opened Menu highlights last used item on Android)
    onOpened: if (isAndroid) currentIndex = -1
    // Workaround for QTBUG-69540 (Menu highlights disabled item on click)
    onCurrentIndexChanged: {
        if (isAndroid || currentIndex < 0)
            return
        for (var i = currentIndex; i < count; ++i) {
            var item = itemAt(i)
            if (item && item.enabled) {
                currentIndex = i
                return
            }
        }
        currentIndex = -1
    }
}
