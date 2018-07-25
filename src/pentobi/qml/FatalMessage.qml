//-----------------------------------------------------------------------------
/** @file pentobi/qml/FatalMessage.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.4

MessageDialog {
    modal: true
    closePolicy: Popup.CloseOnEscape
    onClosed: Qt.quit()
}
