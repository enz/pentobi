//-----------------------------------------------------------------------------
/** @file pentobi/DialogButtonBoxOkCancel.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Controls

DialogButtonBox {
    defaultButton: buttonOk

    ButtonCancel { }
    ButtonOk { id: buttonOk }
}
