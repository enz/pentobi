//-----------------------------------------------------------------------------
/** @file pentobi/ButtonOk.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

/** Replacement for standard button Dialog.Ok, which does not provide
    translations for all needed languages. */
Button {
    text: qsTr("OK")
    DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
}
