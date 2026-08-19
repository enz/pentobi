//-----------------------------------------------------------------------------
/** @file pentobi/+Material/PentobiApplicationBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

ApplicationWindow {
    property bool isDark: true

    Material.theme: Material.Dark
    Material.primary: Material.Grey
    Material.accent: Material.BlueGrey
}
