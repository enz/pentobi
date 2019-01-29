//-----------------------------------------------------------------------------
/** @file pentobi/qml/ButtonCancel.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.2

Button {
    text: qsTr("Cancel")
    DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
}
