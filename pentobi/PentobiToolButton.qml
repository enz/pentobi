//-----------------------------------------------------------------------------
/** @file pentobi/PentobiToolButton.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

PentobiToolButtonBase {
    // Base name of icon file from Pentobi resources
    property string iconSource

    flat: true
    focusPolicy: Qt.NoFocus
    display: AbstractButton.IconOnly
    icon {
        // org.kde.breeze needs absolute path (KDE bug 524343)
        source: "qrc:/qt/qml/PentobiGui/icons/%1/%2.svg".arg(isDark ? "dark" : "light").arg(iconSource)
        // We don't use automatic icon colors because the icons have a high
        // contrast in most styles that distracts from the Blokus board.
        // Also icon coloring is broken in org.kde.breeze (KDE bug 524344).
        color: "transparent"
        width: 16
        height: 16
    }
    hoverEnabled: ! isMobile
    ToolTip.visible: ToolTip.text !== "" && hovered
    ToolTip.delay: 1000
    ToolTip.timeout: 5000
}
