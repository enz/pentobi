//-----------------------------------------------------------------------------
/** @file pentobi/+Fusion/PentobiApplicationBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

ApplicationWindow {
    // Fusion is currently broken and always uses light colors even if
    // Application.styleHints.colorScheme == Qt.Dark (QTBUG-148413).
    // Our best guess if this bug still exists is checking the palette,
    // although that also could be broken (compare Basic style QTBUG-148414).
    property bool isDark: palette.window.hslLightness < 0.5
}
