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
        anchors.fill: parent
    }
    Shortcut {
        sequence: "Ctrl+W"
        onActivated: hide()
    }
}
