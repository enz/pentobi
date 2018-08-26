//-----------------------------------------------------------------------------
/** @file pentobi/qml/Dialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import QtQuick.Controls 2.2

Dialog {
    property real maxContentWidth:
        rootWindow.width - leftPadding - rightPadding

    function centerDialog() {
        // Don't bind x and y because that can cause a binding loop if the
        // application window is interactively resized
        if (ApplicationWindow.window) {
            x = (ApplicationWindow.window.width - width) / 2
            y = (ApplicationWindow.window.height - height) / 2
        }
    }

    focus: true
    clip: true
    closePolicy: Popup.CloseOnEscape
    onOpened: centerDialog()
    onWidthChanged: centerDialog()
    onHeightChanged: centerDialog()
    ApplicationWindow.onWindowChanged:
        if (ApplicationWindow.window) {
            ApplicationWindow.window.onWidthChanged.connect(centerDialog)
            ApplicationWindow.window.onHeightChanged.connect(centerDialog)
        }
    Component.onCompleted:
        if (! isDesktop)
            // Save some screen space on smartphones
            title = ""
}
