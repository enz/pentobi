//-----------------------------------------------------------------------------
/** @file pentobi/PentobiMenuItem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

MenuItem {
    // Add extra transparency as a workaround for QTBUG-119198 (Disabled menu
    // item indistinguishable from enabled one). Note that this workaround
    // might break in the future because the existence of MenuItem.opacity
    // is not documented.
    opacity: workaroundQtBug119198 && ! enabled ? 0.5 : 1
}
