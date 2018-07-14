import QtQuick 2.0
import QtQuick.Controls 2.2
import "." as Pentobi

Menu {
    // implicitWidth should be the same as in Pentobi.MenuItem
    implicitWidth: Math.min(font.pixelSize * (isDesktop ? 22 : 18), rootWindow.width)
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    delegate: Pentobi.MenuItem { }
    background: Rectangle {
        color: "white"
        border.color: "black"
        radius: 2
    }
    // On Android, currentIndex is not reset when closing menu (last tested with Qt 5.11.1)
    onVisibleChanged: if (isAndroid) currentIndex = -1
}
