//-----------------------------------------------------------------------------
/** @file pentobi/qml/PentobiButton.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Window
import QtQuick.Controls

/** Custom tool button.
    Adds a hover effect on the desktop and contains a workaround for
    QTBUG-30801 (tooltip not shown when the button is disabled; the bug has
    been fixed in Qt 6.4 but still doesn't work reliably when the enabled
    status changes while moving the mouse, so we keep our implementation) */
Item {
    id: root

    property string toolTipText

    property alias source: image.source
    property alias action: button.action
    property alias enabled: button.enabled
    property alias down: button.down
    property alias pressed: button.pressed
    property alias autoRepeat: button.autoRepeat
    property alias autoRepeatInterval: button.autoRepeatInterval

    property bool _effectiveHovered:
        isDesktop && toolTipArea.containsMouse && enabled
    property bool _inhibitToolTip

    signal clicked()

    implicitWidth: button.implicitWidth
    implicitHeight: button.implicitHeight

    ToolButton {
        id: button

        padding: Math.min(isDesktop ? 7 : 15,
                          0.03 * rootWindow.contentItem.height,
                          0.03 * rootWindow.contentItem.width)
        opacity: enabled ? 0.55 : 0.25
        hoverEnabled: false
        display: AbstractButton.IconOnly
        focusPolicy: Qt.NoFocus
        flat: true
        contentItem: Item {
            // We want the icon about the same size as the font, but use
            // multipliers of 8 for better pixel alignment.
            implicitWidth:
                Math.round(1.2 * font.pixelSize * Screen.devicePixelRatio / 8)
                / Screen.devicePixelRatio * 8
            implicitHeight: implicitWidth

            Image {
                id: image

                anchors.fill: parent
            }
        }
        background: Item {
            id: backgroundItem

            function startClickedAnimation() { pressedAnimation.restart() }

            Rectangle {
                id: pressedBackground

                anchors.fill: parent
                radius: 0.05 * width
                color: theme.colorButtonPressed
                opacity: button.down ? 1 : 0
            }
            Rectangle {
                anchors.fill: parent
                radius: 0.05 * width
                color: _effectiveHovered && ! button.down ?
                           theme.colorButtonHovered : "transparent"
                border.color: _effectiveHovered ? theme.colorButtonBorder
                                                : "transparent"
            }
            NumberAnimation {
                id: pressedAnimation

                target: pressedBackground
                property: "opacity"
                from: 1; to: 0
                duration: isAndroid ? 300 : 0
                easing.type: Easing.InQuad
            }
        }
        onPressed: _inhibitToolTip = true
        onClicked: { backgroundItem.startClickedAnimation(); root.clicked() }

        Behavior on opacity {
            NumberAnimation { duration: gameView.animationDurationFast }
        }
    }
    MouseArea {
        id: toolTipArea

        anchors.fill: parent
        visible: isDesktop
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onExited: _inhibitToolTip = false
        ToolTip.text: toolTipText
        ToolTip.visible: containsMouse && ToolTip.text
                         && ! root._inhibitToolTip
        ToolTip.delay: 1000
        ToolTip.timeout: 7000
    }
}
