//-----------------------------------------------------------------------------
/** @file pentobi/DialogLoader.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Loader {
    property Component component

    function get() {
        if (! item) sourceComponent = component
        return item
    }
    function open() { get().open() }
}
