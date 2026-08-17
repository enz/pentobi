//-----------------------------------------------------------------------------
/** @file pentobi/PentobiButton.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

Button {
    // Base name of icon file from Pentobi resources
    property string iconSource

    flat: true
    focusPolicy: Qt.NoFocus
    display: AbstractButton.IconOnly
    icon {
        // org.kde.breeze needs absolute path (KDE bug 524343)
        source: "qrc:/qt/qml/PentobiGui/icons/%1/%2.svg".arg(theme.isDark ? "dark" : "light").arg(iconSource)
        // org.kde.breeze doesn't suppert automatic icon coloring (KDE bug 524344)
        color: "transparent"
        width: 16
        height: 16
    }
    ToolTip.visible: ToolTip.text !== "" && hovered
    ToolTip.delay: 1000
    ToolTip.timeout: 5000
}
