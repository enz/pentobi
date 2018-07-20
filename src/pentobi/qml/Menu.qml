import QtQuick 2.0
import QtQuick.Controls 2.2
import "Controls.js" as PentobiControls
import "." as Pentobi

Menu {
    function addMnemonic(text, mnemonic) { return PentobiControls.addMnemonic(text, mnemonic) }

    // width should be the same as in Pentobi.MenuItem
    width: Math.min(font.pixelSize * (isDesktop ? 22 : 18), rootWindow.width)
    cascade: isDesktop
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    delegate: Pentobi.MenuItem { }
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
