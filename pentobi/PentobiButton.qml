//-----------------------------------------------------------------------------
/** @file pentobi/PentobiButton.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Window
import QtQuick.Controls

/** Custom tool button.
    Contains workarounds for using ToolButton directly:
    - icon.source resizes loaded images before scaling to HDPI, so they look
      blurry in HDPI displays (Qt 6.9)
    - Issues with default button size or padding in some styles
    - Using icon.source sometimes resizes images differently even if thes have
      the same source size (Qt 6.9, Android)
    If you make changes, make sure the toolbar and mobile view navigation panel
    work at least in the default styles used by Pentobi (Fusion or
    org.kde.breeze on Linux, Material on Android, FluentWinUI3 on Windows) */
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

    signal clicked()

    implicitWidth: button.implicitWidth
    implicitHeight: button.implicitHeight

    ToolButton {
        id: button

        padding: Math.min(isDesktop ? 7 : 15,
                          0.03 * rootWindow.contentItem.height,
                          0.03 * rootWindow.contentItem.width)
        hoverEnabled: ! isAndroid
        display: AbstractButton.IconOnly
        focusPolicy: Qt.NoFocus
        flat: true
        contentItem: Item {
            // We want the icon about the same size as the font, but use
            // multipliers of 8 for better pixel alignment (source images
            // are 64x64 aligned to 16x16 grid)
            implicitWidth:
                Math.round((globalStyle === "Material" ? 1.3 : 1.2)
                           * font.pixelSize * Screen.devicePixelRatio / 8)
                / Screen.devicePixelRatio * 8
            implicitHeight: implicitWidth
            opacity: button.enabled ? 0.55 : 0.25

            Image {
                id: image

                anchors.fill: parent
            }
            Behavior on opacity {
                NumberAnimation { duration: gameView.animationDurationFast }
            }
        }
        background: Item {
            id: backgroundItem

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
                color:
                    button.hovered && ! button.down ?
                           theme.colorButtonHovered : "transparent"
                border.color:
                    button.hovered && button.enabled ?
                        theme.colorButtonBorder : "transparent"
            }
        }
        ToolTip.text: root.toolTipText
        ToolTip.visible: toolTipText !== "" && hovered
        ToolTip.delay: 1000
        ToolTip.timeout: 7000
        onClicked: root.clicked()
    }
}
