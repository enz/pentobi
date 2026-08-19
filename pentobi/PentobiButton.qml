//-----------------------------------------------------------------------------
/** @file pentobi/PentobiButton.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

// See PentobiToolButton for comments on the implementation
Button {
    property string iconSource

    flat: true
    focusPolicy: Qt.NoFocus
    display: AbstractButton.IconOnly
    icon {
        source: "qrc:/qt/qml/PentobiGui/icons/%1/%2.svg".arg(isDark ? "dark" : "light").arg(iconSource)
        color: "transparent"
        width: 16
        height: 16
    }
    opacity: {
        if (globalStyle == "org.kde.breeze" || globalStyle == "org.kde.desktop")
            return 1
        return enabled ? 1 : 0.4
    }
    ToolTip.visible: ToolTip.text !== "" && hovered
    ToolTip.delay: 1000
    ToolTip.timeout: 5000
}
