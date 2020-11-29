//-----------------------------------------------------------------------------
/** @file pentobi/qml/PentobiDialogButtonBox.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.4

DialogButtonBox {
    // In Qt 5.11, undefined alignment can cause a binding loop for
    // implicitWidth of the dialog in default style
    alignment: Qt.AlignRight
}
