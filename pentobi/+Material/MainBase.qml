//-----------------------------------------------------------------------------
/** @file pentobi/+Material/MainBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

ApplicationWindow {
    property bool isDark: true

    // Light mode is broken on some platforms (e.g. light palette but
    // Application.styleHints.colorScheme dark on postmarketOS 26.06/Phosh with
    // default style selected), so we only support dark mode right now, which
    // looks much better for the Blokus app on phones anyway
    Material.theme: Material.Dark
    Material.primary: Material.Grey
    Material.accent: Material.BlueGrey
}
