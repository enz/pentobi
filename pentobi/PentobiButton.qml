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
    - Style org.kde.breeze does not support icons in resources even if
      QIcon::setThemeSearchPaths() is used (Qt 6.11)
    - Using icon.source sometimes resizes images differently even if they have
      the same source size (Qt 6.9, Android)
    - Issues with default button size or padding in some styles
    If you make changes, make sure the toolbar and mobile view navigation panel
    work at least in the default styles used by Pentobi (Fusion or
    org.kde.breeze on Linux, Material on Android, FluentWinUI3 on Windows) */
Item {
    id: root

    property string toolTipText

    property alias source: image.source
    property alias action: button.action
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
        display: AbstractButton.IconOnly
        focusPolicy: Qt.NoFocus
        flat: true
        contentItem: Item {
            implicitWidth: 16
            implicitHeight: 16

            Image {
                id: image

                anchors.fill: parent
                opacity: button.enabled ? 1 : 0.5
                Behavior on opacity {
                    NumberAnimation {
                        duration: gameView.animationDurationFast
                    }
                }
            }
        }
        background: Item {
            id: backgroundItem

            implicitWidth: 16
            implicitHeight: 16

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
        ToolTip.timeout: 5000
        onClicked: {
            ToolTip.toolTip.hide()
            root.clicked()
        }
    }
}
