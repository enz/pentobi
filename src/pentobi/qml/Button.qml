import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 2.3

ToolButton {
    id: root

    // Icon size 22x22, pixel-alignment optimized for this size
    property url imageSource

    opacity: root.enabled ? 1 : 0.5
    display: AbstractButton.IconOnly
    icon {
        source: imageSource
        color: theme.colorIcon
        width: Screen.pixelDensity < 5 ? 22 : 5 * Screen.pixelDensity
        height: Screen.pixelDensity < 5 ? 22 : 5 * Screen.pixelDensity
    }
    focusPolicy: Qt.NoFocus
    flat: true
    background: Rectangle {
        anchors.fill: root
        visible: down
        color: theme.backgroundButtonPressed
    }
    onPressed: if (isDesktop) toolTipSuppressTimer.restart()
    hoverEnabled: true
    ToolTip.visible:
        ToolTip.text && isDesktop && hovered && ! toolTipSuppressTimer.running
        && ! overlay.children.length === 0
    ToolTip.delay: 900
    ToolTip.timeout: 9000

    // If the button is pressed and released, we don't want to show the tooltip
    // to show again
    Timer {
        id: toolTipSuppressTimer

        interval: 9000
    }
}
