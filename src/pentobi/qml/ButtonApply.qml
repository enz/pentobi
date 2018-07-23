//-----------------------------------------------------------------------------
/** @file pentobi/qml/ButtonApply.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls 2.2
import "Controls.js" as PentobiControls

Button {
    text: PentobiControls.addMnemonic(
              qsTr("Apply"),
              //: Mnemonic for button Apply. Leave empty for no mnemonic.
              qsTr("A"))
    DialogButtonBox.buttonRole: DialogButtonBox.ApplyRole
}
