//-----------------------------------------------------------------------------
/** @file pentobi/qml/Button.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 2.3

ToolButton {
    id: root

    // See ButtonToolTip
    property bool buttonToolTipHovered
    property bool effectiveHovered: Qt.styleHints.useHoverEffects && isDesktop
                                    && buttonToolTipHovered && enabled

    // We use SVG icon sources of size 22x22 and want the icon about 1.7x the
    // font size, but use multipliers in quarter-size  steps (5.5) for better
    // pixel alignment. Minimum size is half the source size. Note that on some
    // Android 4.2 devices, Qt 5.11 reports a much too low pixelDensity (e.g.
    // pixelDensity=4.2, devicePixelRatio=1.5 on a 4.0" 480x800 device) but
    // uses a reasonable font size, so deriving the size directly from
    // pixelDensity and devicePixelRatio is not a good idea.
    function getIconSize() {
        return Math.max(
                    Math.round(
                        1.7 * font.pixelSize * Screen.devicePixelRatio / 5.5)
                    / Screen.devicePixelRatio * 5.5,
                    Screen.devicePixelRatio * 11)
    }

    opacity: root.enabled ? theme.opacitySubduedText : 0.5 * theme.opacitySubduedText
    hoverEnabled: false
    display: AbstractButton.IconOnly
    icon {
        color: theme.colorText
        width: getIconSize()
        height: getIconSize()
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
