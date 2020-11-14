//-----------------------------------------------------------------------------
/** @file pentobi/qml/FatalMessage.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

MessageDialog {
    onClosed:
        if (isAndroid)
            androidUtils.quit()
        else
            Qt.quit()
}
