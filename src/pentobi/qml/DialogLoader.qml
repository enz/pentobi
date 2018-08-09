//-----------------------------------------------------------------------------
/** @file pentobi/qml/DialogLoader.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Loader {
    function get() {
        if (! item)
            sourceComponent = resources[0]
        return item
    }
    function open() { get().open() }
}
