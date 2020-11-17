//-----------------------------------------------------------------------------
/** @file pentobi/qml/Button.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 2.3

/** Custom tool button.
    Adds a hover effect on the desktop and contains a workaroround for
    QTBUG-30801 (tooltip not shown when the button is disabled). */
Item {
    id: root

    property string toolTipText

    property alias action: button.action
    property alias icon: button.icon
    property alias down: button.down
    property alias pressed: button.pressed
    property alias autoRepeat: button.autoRepeat
    property alias autoRepeatInterval: button.autoRepeatInterval

    property bool _toolTipHovered: toolTipArea.containsMouse
    property bool _effectiveHovered: isDesktop && _toolTipHovered && enabled
    property bool _inhibitToolTip

    // We want the icon about the same size as the font, but use multipliers in
    // quarter-size steps (4) for better pixel alignment.
    property int _iconSize:
        Math.round(1.2 * font.pixelSize * Screen.devicePixelRatio / 4)
        / Screen.devicePixelRatio * 4

    signal clicked()

    implicitWidth: Math.min(_iconSize + (isDesktop ? 14 : 30),
                            0.11 * rootWindow.contentItem.height,
                            0.13 * rootWindow.contentItem.width)
    implicitHeight: implicitWidth

    ToolButton {
        id: button

        anchors.fill:parent
        opacity: enabled ? 0.55 : 0.25
        hoverEnabled: false
        display: AbstractButton.IconOnly
        icon {
            color: theme.colorText
            width: _iconSize
            height: _iconSize
        }
        focusPolicy: Qt.NoFocus
        flat: true
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
