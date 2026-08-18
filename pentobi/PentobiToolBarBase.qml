//-----------------------------------------------------------------------------
/** @file pentobi/PentobiToolBarBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

/** Style-dependent functionality of PentobiToolBar. */
ToolBar {

    // We don't want the toolbar to be visually separated from the main window
    // content. Also, Material.Dark uses a wrong toolbar color (Qt 6.11).
    background: Item { }
}
