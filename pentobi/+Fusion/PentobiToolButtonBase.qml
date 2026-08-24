//-----------------------------------------------------------------------------
/** @file pentobi/+Fusion/PentobiToolButtonBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

ToolButton {
    opacity: enabled ? 1 : 0.4
    // Fusion uses no padding on icon-only toolbuttons (Qt 6.11)
    padding: 8
}