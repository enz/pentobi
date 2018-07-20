//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuItem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "Controls.js" as PentobiControls

MenuItem {
    id: root

    function addMnemonic(text, mnemonic) { return PentobiControls.addMnemonic(text, mnemonic) }

    // width should be the same as in Pentobi.Menu
    width: Math.min(font.pixelSize * (isDesktop ? 22 : 18), rootWindow.width)
}
