//-----------------------------------------------------------------------------
/** @file pentobi/PentobiMenuItem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

MenuItem {
    // Add extra transparency as a workaround for QTBUG-119198 (Disabled menu
    // item indistinguishable from enabled one).
    opacity: workaroundQtBug119198 && ! enabled ? 0.65 : 1
}
