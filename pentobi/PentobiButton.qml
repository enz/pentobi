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
    - Using icon.source sometimes resizes images differently even if they have
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

    // We want the icon about the same size as the font, but use
    // multipliers of 8 for better pixel alignment (source images
    // are 64x64 aligned to 16x16 grid)
    property real _imageSize:
        Math.round((globalStyle === "Material" ? 1.3 : 1.2)
                   * font.pixelSize * Screen.devicePixelRatio / 8)
        / Screen.devicePixelRatio * 8

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
            implicitWidth: _imageSize
            implicitHeight: _imageSize

            Image {
                id: image

                anchors.fill: parent
                opacity: button.enabled ? 1 : 0.5
            }
            Behavior on opacity {
                NumberAnimation { duration: gameView.animationDurationFast }
            }
        }
        background: Item {
            id: backgroundItem

            implicitWidth:
                Math.round((globalStyle === "Material" ? 1.3 : 1.2)
                           * font.pixelSize * Screen.devicePixelRatio / 8)
                / Screen.devicePixelRatio * 8
            implicitHeight: implicitWidth

            Rectangle {
                anchors.fill: parent
                radius: 0.05 * width
                color: theme.colorButtonPressed
                opacity: button.down ? 1 : 0

                Behavior on opacity {
                    NumberAnimation { duration: gameView.animationDurationFast }
                }
            }
            Rectangle {
                anchors.fill: parent
                radius: 0.05 * width
                color: theme.colorButtonHovered
                visible: button.enabled && button.hovered
            }
        }
        ToolTip.text: root.toolTipText
        ToolTip.visible: toolTipText !== "" && hovered
        ToolTip.delay: 1000
        ToolTip.timeout: 7000
        onClicked: root.clicked()
    }
}
