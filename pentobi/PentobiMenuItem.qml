//-----------------------------------------------------------------------------
/** @file pentobi/PentobiMenuItem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

MenuItem {
    // Prevents unwanted appearances of the on-screen keyboard in
    // postmarketOS/Phosh (Aug 2026) when popups (Menu, Dialog) are opened or
    // closed
    focusPolicy: isMobile ? Qt.NoFocus : Qt.StrongFocus
}
