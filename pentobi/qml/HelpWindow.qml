//-----------------------------------------------------------------------------
/** @file pentobi/qml/HelpWindow.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    property real defaultWidth:
        Math.min(font.pixelSize * 40, Screen.desktopAvailableWidth)
    property real defaultHeight:
        Math.min(font.pixelSize * 50, Screen.desktopAvailableHeight)

    width: defaultWidth; height: defaultHeight
    minimumWidth: 240; minimumHeight: 240
    x: (Screen.width - defaultWidth) / 2
    y: (Screen.height - defaultHeight) / 2
    title: qsTr("Pentobi Help")

    HelpViewer {
        id: helpViewer

        anchors.fill: parent
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
