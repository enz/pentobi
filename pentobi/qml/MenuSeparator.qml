//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuSeparator.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3

MenuSeparator {
    // Qt 5.12.0 alpha doesn't set the width of menu items
    width: parent.width
    // Default implicitWidth is too large. Since we either use fixed-width
    // menus or compute the menu width from the maximum implicitWidth of the
    // items, we set implicitWidth of the separators to 0
    implicitWidth: 0
}
