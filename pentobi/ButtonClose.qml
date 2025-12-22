//-----------------------------------------------------------------------------
/** @file pentobi/ButtonClose.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

/** Replacement for standard button Dialog.Close, which does not provide
    translations for all needed languages. */
Button {
    text: qsTr("Close")
    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
}
