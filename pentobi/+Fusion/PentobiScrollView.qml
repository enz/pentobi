//-----------------------------------------------------------------------------
/** @file pentobi/+Fusion/PentobiScrollView.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    // Workaround for QTBUG-140033 (Scrollbar not painted in Fusion style,
    // Qt 6.11)
    ScrollBar.vertical.contentItem: Rectangle {
        implicitWidth: 6
        radius: 3
        color: theme.isDark ? "white" : "black"
        opacity:
            if (root.ScrollBar.vertical.pressed)
                return 0.4
            else if (root.ScrollBar.vertical.hovered)
                return 0.3
            else if (root.ScrollBar.vertical.size < 1)
                return 0.2
            else
                return 0
    }
}
