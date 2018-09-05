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
    property real maxContentHeight: {
        var h = rootWindow.height - topPadding - bottomPadding
        if (header && header.visible)
            h -= header.implicitHeight
        if (footer && footer.visible)
            h -= footer.implicitHeight
        return h
    }
    property real minContentWidth:
        // Match window width on mobile devices within reason
        isDesktop ? 0 : Math.min(60 * font.pixelSize, maxContentWidth)

    function centerDialog() {
        // Don't bind x and y because that can cause a binding loop if the
        // application window is interactively resized
        if (ApplicationWindow.window) {
            x = (ApplicationWindow.window.width - width) / 2
            y = (ApplicationWindow.window.height - height) / 2
        }
    }
    // Has any dialog button the active focus (see also Pentobi.Actions.returnKey)
    function hasButtonFocus() {
        for (var i = 0; i < footer.contentChildren.length; ++i)
            if (footer.contentChildren[i].activeFocus)
                return true
        return false
    }

    focus: true
    clip: true
    closePolicy: Popup.CloseOnEscape
    onOpened: {
        centerDialog()
        rootWindow.dialogs.push(this)
    }
    onClosed: rootWindow.dialogs.splice(rootWindow.dialogs.indexOf(this), 1)
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
