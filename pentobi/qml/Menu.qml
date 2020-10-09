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
    function addShortcut(text, shortcut) { return PentobiControls.addShortcut(text, shortcut) }

    width: Math.min(font.pixelSize * 24, rootWindow.contentItem.width)
    cascade: isDesktop
    closePolicy: isDesktop ?
                     Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
                   : Popup.CloseOnEscape | Popup.CloseOnPressOutside
    delegate: Pentobi.MenuItem { }
    background: Rectangle {
        // Note that MenuItem in Qt 5.11 does neither fully use the system
        // palette, nor make its actually used colors available.
        color: isDesktop ? palette.window : palette.base
        border.color: palette.mid
    }
    // Workaround for QTBUG-69541 (Opened Menu highlights last used item on Android)
    onOpened: if (isAndroid) currentIndex = -1
    // Workaround for QTBUG-69540 (Menu highlights disabled item on click).
    // Also part of workaround for QTBUG-70181, see Pentobi.MenuItem.Keys.onPressed
    onCurrentIndexChanged: {
        if (isAndroid || currentIndex < 0)
            return
        var i
        for (i = currentIndex; i < count; ++i)
            if (itemAt(i) instanceof MenuItem && itemAt(i).enabled) {
                currentIndex = i
                return
            }
        for (i = currentIndex - 1; i >= 0; --i)
            if (itemAt(i) instanceof MenuItem && itemAt(i).enabled) {
                currentIndex = i
                return
            }
        currentIndex = -1
    }
    Component.onCompleted: {
        // Sanity checks for shortcuts
        if (! isDebug || ! isDesktop)
            return
        var allShortcuts = []
        var i, j, text, pos, shortcut, textWithoutShortcut
        for (i = 0; i < count; ++i) {
            if (itemAt(i))
                text = itemAt(i).text
            else if (menuAt(i))
                text = menuAt(i).title
            if (! text)
                continue
            pos = text.indexOf("&")
            if (pos < 0 || pos === text.length - 1) {
                textWithoutShortcut = text
                continue
            }
            shortcut = text.substr(pos + 1, 1).toLowerCase()
            for (j = 0; j < allShortcuts.length; ++j)
                if (allShortcuts[j] === shortcut)
                    console.warn("Duplicate menu shortcut:", text)
            allShortcuts.push(shortcut)
        }
        if (allShortcuts.length > 0 && textWithoutShortcut)
            console.warn("Missing menu shortcut:", textWithoutShortcut)
    }
}
