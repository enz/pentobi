//-----------------------------------------------------------------------------
/** @file pentobi/PentobiToolButtonBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

/** Style-dependent functionality of PentobiToolButton.
    Use file selectors to override. */
ToolButton {
    // Most styles don't render disabled buttons with custom icons differently
    // from enabled ones (Qt 6.11)
    opacity: enabled ? 1 : 0.4
}