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

    // See ButtonToolTip
    property bool buttonToolTipHovered
    property bool effectiveHovered:
        Qt.styleHints.useHoverEffects && isDesktop && buttonToolTipHovered

    opacity: root.enabled ? theme.opacitySubduedText : 0.5 * theme.opacitySubduedText
    hoverEnabled: false
    display: AbstractButton.IconOnly
    icon {
        color: theme.colorText
        width: Screen.pixelDensity < 5 ? 22 : 5 * Screen.pixelDensity
        height: Screen.pixelDensity < 5 ? 22 : 5 * Screen.pixelDensity
    }
    focusPolicy: Qt.NoFocus
    flat: true
    background: Rectangle {
        anchors.fill: root
        radius: 0.05 * width
        color: down ? theme.colorButtonPressed :
                      effectiveHovered ? theme.colorButtonHovered
                                       : "transparent"
        border.color: down || effectiveHovered ? theme.colorButtonBorder
                                               : "transparent"
    }
}
