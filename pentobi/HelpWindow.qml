//-----------------------------------------------------------------------------
/** @file pentobi/HelpWindow.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Window
import Qt.labs.settings

Window {
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
        sequence: StandardKey.Close
        onActivated: hide()
    }
    Shortcut {
        sequence: StandardKey.MoveToStartOfLine
        onActivated: helpViewer.goHome()
    }
    Shortcut {
        sequence: StandardKey.MoveToNextPage
        onActivated: helpViewer.scrollPageDown()
    }
    Shortcut {
        sequence: StandardKey.MoveToPreviousPage
        onActivated: helpViewer.scrollPageUp()
    }
    Shortcut {
        sequence: StandardKey.MoveToNextLine
        onActivated: helpViewer.scrollDown()
    }
    Shortcut {
        sequence: StandardKey.MoveToPreviousLine
        onActivated: helpViewer.scrollUp()
    }
    Shortcut {
        sequence: StandardKey.MoveToNextChar
        onActivated: helpViewer.nextPage()
    }
    Shortcut {
        sequence: StandardKey.MoveToPreviousChar
        onActivated: helpViewer.prevPage()
    }
}
