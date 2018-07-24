//-----------------------------------------------------------------------------
/** @file pentobi/qml/ButtonClose.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.2
import "Controls.js" as PentobiControls

Button {
    text: PentobiControls.addMnemonic(
              qsTr("Close"),
              //: Mnemonic for button OK. Leave empty for no mnemonic.
              qsTr("C"))
    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
}
