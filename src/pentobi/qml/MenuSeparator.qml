//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuSeparator.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.3

MenuSeparator {
    // implicitWidth of MenuSeparator in Qt 5.11 is too large
    implicitWidth: 5 * font.pixelSize
}
