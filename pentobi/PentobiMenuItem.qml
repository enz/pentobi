//-----------------------------------------------------------------------------
/** @file pentobi/PentobiMenuItem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

MenuItem {
    focusPolicy: workaroundOskBug ? Qt.NoFocus : Qt.StrongFocus
}
