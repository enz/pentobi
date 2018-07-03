import QtQuick.Controls 2.2
import QtQuick.Window 2.0

Menu {
    cascade: ! isAndroid
    width: Math.min(rootWindow.font.pixelSize * 18, rootWindow.width)
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
}
