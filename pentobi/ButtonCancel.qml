//-----------------------------------------------------------------------------
/** @file pentobi/ButtonCancel.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

Button {
    text: qsTr("Cancel")
    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
}
