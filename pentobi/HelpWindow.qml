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

    minimumWidth: 240; minimumHeight: 240
    title: qsTr("Pentobi Help")
    onVisibleChanged:
        if (! visible && visibility == Window.Windowed) {
            settings.x = x
            settings.y = y
            settings.width = width
            settings.height = height
        }

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
    Shortcut {
        sequences: [ StandardKey.MoveToStartOfLine ]
        onActivated: helpViewer.goHome()
    }
    Shortcut {
        sequences: [ StandardKey.MoveToNextPage ]
        onActivated: helpViewer.scrollPageDown()
    }
    Shortcut {
        sequences: [ StandardKey.MoveToPreviousPage ]
        onActivated: helpViewer.scrollPageUp()
    }
    Shortcut {
        // Doesn't work with Qt 6.8
        sequences: [ StandardKey.MoveToNextLine ]
        onActivated: helpViewer.scrollDown()
    }
    Shortcut {
        // Doesn't work with Qt 6.8
        sequences: [ StandardKey.MoveToPreviousLine ]
        onActivated: helpViewer.scrollUp()
    }
    Shortcut {
        // Doesn't work with Qt 6.8
        sequences: [ StandardKey.MoveToNextChar ]
        onActivated: helpViewer.nextPage()
    }
    Shortcut {
        // Doesn't work with Qt 6.8
        sequences: [ StandardKey.MoveToPreviousChar ]
        onActivated: helpViewer.prevPage()
    }
}
