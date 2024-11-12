//-----------------------------------------------------------------------------
/** @file pentobi/MenuHelp.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

PentobiMenu {
    title: qsTr("Help")

    PentobiMenuItem {
        action: actionHelp
    }
    PentobiMenuItem {
        text: qsTr("About Pentobi")
        onTriggered: aboutDialog.open()
    }
}
