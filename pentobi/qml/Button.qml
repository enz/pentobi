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

    // Used instead of attached tooltip because of QTBUG-30801 (tooltip not
    // shown when the button is disabled).
    property string toolTipText

    property bool _toolTipHovered
    property bool _effectiveHovered: isDesktop && _toolTipHovered && enabled
    property bool _inhibitToolTip

    implicitWidth: Math.min(getIconSize() + (isDesktop ? 14 : 30),
                            0.11 * rootWindow.contentItem.height,
                            0.13 * rootWindow.contentItem.width)
    implicitHeight: implicitWidth

    // We use SVG icon sources of size 16x16 and want the icon about the same
    // size as the font, but use multipliers in quarter-size steps (4) for
    // better pixel alignment. Minimum size is 8. Note that on some Android 4.2
    // devices, Qt 5.11 reports a much too low pixelDensity (e.g.
    // pixelDensity=4.2, devicePixelRatio=1.5 on a 4.0" 480x800 device) but
    // uses a reasonable font size, so deriving the size directly from
    // pixelDensity and/or devicePixelRatio is not a good idea.
    function getIconSize() {
        return Math.max(
                    Math.round(
                        1.2 * font.pixelSize * Screen.devicePixelRatio / 4)
                    / Screen.devicePixelRatio * 4,
                    Screen.devicePixelRatio * 8)
    }

    Behavior on opacity {
        NumberAnimation { duration: gameView.animationDurationFast }
    }

    opacity: root.enabled ? 0.55 : 0.25
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
        radius: 0.05 * width
        color: down ? theme.colorButtonPressed :
                      _effectiveHovered ? theme.colorButtonHovered
                                        : "transparent"
        border.color: down || _effectiveHovered ? theme.colorButtonBorder
                                                : "transparent"
    }
    onPressed: _inhibitToolTip = true

    MouseArea {
        id: toolTipArea

        parent: root.parent
        x: root.x
        y: root.y
        width: root.width
        height: root.height
        visible: isDesktop
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onExited: root._inhibitToolTip = false
        ToolTip.text: root.toolTipText
        ToolTip.visible: containsMouse && ToolTip.text
                         && ! root._inhibitToolTip
        ToolTip.delay: 1000
        ToolTip.timeout: 7000
        Component.onCompleted:
            root._toolTipHovered = Qt.binding(function() {
                return containsMouse })
    }
}
