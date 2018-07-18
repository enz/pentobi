import QtQuick 2.0
import QtQuick.Controls 2.2
import "Controls.js" as PentobiControls
import "." as Pentobi

Menu {
    function addMnemonic(text, mnemonic) { return PentobiControls.addMnemonic(text, mnemonic) }

    // implicitWidth should be the same as in Pentobi.MenuItem
    implicitWidth: Math.min(font.pixelSize * (isDesktop ? 22 : 18), rootWindow.width)
    cascade: isDesktop
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    delegate: Pentobi.MenuItem { }
    // On Android, currentIndex is not reset when closing menu (last tested with Qt 5.11.1)
    onVisibleChanged: if (isAndroid) currentIndex = -1
}
