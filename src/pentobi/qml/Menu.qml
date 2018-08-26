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

    property bool dynamicWidth: isDesktop

    width: {
        if (! dynamicWidth)
            return Math.min(font.pixelSize * 17, rootWindow.width)
        var maxWidth = 0
        for (var i = 0; i < count; ++i)
            maxWidth = Math.max(maxWidth, itemAt(i).implicitWidth)
        return Math.min(maxWidth, rootWindow.width)
    }
    cascade: isDesktop
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    delegate: Pentobi.MenuItem { }
    background: Rectangle {
        // Note that MenuItem in Qt 5.11 does neither fully use the system
        // palette, nor make its actually used colors available in its own
        // palette.
        color: isDesktop ? palette.window : palette.base
        border.color: palette.mid
    }
    // Workaround for QTBUG-69541 (Opened Menu highlights last used item on Android)
    onOpened: if (isAndroid) currentIndex = -1
    // Workaround for QTBUG-69540 (Menu highlights disabled item on click)
    onCurrentIndexChanged: {
        if (isAndroid || currentIndex < 0)
            return
        for (var i = currentIndex; i < count; ++i)
            if (itemAt(i) instanceof MenuItem && itemAt(i).enabled) {
                currentIndex = i
                return
            }
        currentIndex = -1
    }
    Component.onCompleted: {
        // Sanity checks for mnemonics
        if (! isDebug || ! isDesktop)
            return
        var allMnemonics = []
        var i, j, text, pos, mnemonic, textWithoutMnemonic
        for (i = 0; i < count; ++i) {
            if (itemAt(i))
                text = itemAt(i).text
            else if (menuAt(i))
                text = menuAt(i).title
            if (! text)
                continue
            pos = text.indexOf("&")
            if (pos < 0 || pos === text.length - 1) {
                textWithoutMnemonic = text
                continue
            }
            mnemonic = text.substr(pos + 1, 1).toLowerCase()
            for (j = 0; j < allMnemonics.length; ++j)
                if (allMnemonics[j] === mnemonic)
                    console.warn("Duplicate mnemonic:", text)
            allMnemonics.push(mnemonic)
        }
        if (allMnemonics.length > 0 && textWithoutMnemonic)
            console.warn("No mnemonic:", textWithoutMnemonic)
    }
}
