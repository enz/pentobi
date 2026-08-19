//-----------------------------------------------------------------------------
/** @file pentobi/MainBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

/** Style-dependent functionality of PentobiApplication.
    Use file selectors to override. */
ApplicationWindow {
    property bool isDark: Application.styleHints.colorScheme === Qt.Dark
}
