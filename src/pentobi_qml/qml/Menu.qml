import QtQuick 2.0
import QtQuick.Controls 2.2
import "." as Pentobi

Menu {
    implicitWidth: Math.min(font.pixelSize * (isDesktop ? 21 : 18), rootWindow.width)
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    delegate: Pentobi.MenuItem { }
    background: Rectangle {
        color: "white"
        border.color: "black"
        radius: 2
    }
}
