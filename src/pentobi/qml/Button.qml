//-----------------------------------------------------------------------------
/** @file pentobi/qml/Button.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 2.3

// Icon size should be 22x22, pixel-alignment optimized for this size
ToolButton {
    id: root

    opacity: root.enabled ? 1 : 0.5
    display: AbstractButton.IconOnly
    icon {
        color: theme.colorSubduedText
        width: Screen.pixelDensity < 5 ? 22 : 5 * Screen.pixelDensity
        height: Screen.pixelDensity < 5 ? 22 : 5 * Screen.pixelDensity
    }
    focusPolicy: Qt.NoFocus
    flat: true
    background: Rectangle {
        anchors.fill: root
        visible: down
        color: theme.colorButtonPressed
    }
    onPressed: if (isDesktop) toolTipSuppressTimer.restart()
    hoverEnabled: true
    ToolTip.visible:
        ToolTip.text && isDesktop && hovered && ! toolTipSuppressTimer.running
        && overlay.children.length === 0
    ToolTip.delay: 900
    ToolTip.timeout: 9000

    // If the button is pressed and released, we don't want to show the tooltip
    // to show again
    Timer {
        id: toolTipSuppressTimer

        interval: 9000
    }
}
