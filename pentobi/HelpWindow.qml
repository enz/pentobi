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
    // Note that MoveToNextLine, MoveToPreviousLine, MoveToNextChar and
    // MoveToPreviousChar currently don't work for unknown reasons (last
    // tested with Qt 6.8.0) but the remaining shortcuts are enough for basic
    // navigation without a mouse.
    Shortcut {
        sequences: [ StandardKey.MoveToNextLine ]
        onActivated: helpViewer.scrollDown()
    }
    Shortcut {
        sequences: [ StandardKey.MoveToPreviousLine ]
        onActivated: helpViewer.scrollUp()
    }
    Shortcut {
        sequences: [ StandardKey.MoveToNextChar, StandardKey.MoveToNextWord ]
        onActivated: helpViewer.nextPage()
    }
    Shortcut {
        sequences: [ StandardKey.MoveToPreviousChar, StandardKey.MoveToPreviousWord ]
        onActivated: helpViewer.prevPage()
    }
}
