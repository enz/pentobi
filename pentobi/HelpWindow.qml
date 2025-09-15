//-----------------------------------------------------------------------------
/** @file pentobi/HelpWindow.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtCore
import QtQuick
import QtQuick.Window

Window {
    id: root

    function open() {
        width = settings.width
        height = settings.height
        x = settings.x
        y = settings.y
        show()
        var maxWidth = rootWindow.Screen.desktopAvailableWidth
        var maxHeight = rootWindow.Screen.desktopAvailableHeight
        if (settings.width <= 0 || settings.height <= 0
                || settings.x < 0 || settings.y < 0
                || settings.x + settings.width >= maxWidth
                || settings.y + settings.height >= maxHeight)
        {
            width = Math.min(font.pixelSize * 40, maxWidth)
            height = Math.min(font.pixelSize * 50, maxHeight)
            x = (Screen.width - width) / 2
            y = (Screen.height - height) / 2
        }
    }

    minimumWidth: 240; minimumHeight: 240
    title: qsTr("Pentobi Help")
    color: theme.colorBackground
    onVisibleChanged: {
        if (visibility == Window.Windowed) {
            settings.x = x
            settings.y = y
            settings.width = width
            settings.height = height
        }
    }

    HelpViewer {
        id: helpViewer

        anchors.fill: parent
        onCloseClicked: root.hide()
    }
    Settings {
        id: settings

        property real x
        property real y
        property real width
        property real height

        category: "HelpWindow"
    }
    Shortcut {
        sequences: [ StandardKey.Close ]
        onActivated: root.hide()
    }
}
