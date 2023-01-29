//-----------------------------------------------------------------------------
/** @file pentobi/qml/PentobiDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

Dialog {
    property real maxContentWidth:
        rootWindow.contentItem.width - leftPadding - rightPadding
    property real maxContentHeight: {
        var h = rootWindow.contentItem.height - topPadding - bottomPadding
        if (header && header.visible)
            h -= header.implicitHeight
        if (footer && footer.visible)
            h -= footer.implicitHeight
        return h
    }
    property real minContentWidth:
        // Match 90% window width on mobile devices within reason
        isDesktop ? 0 : Math.min(27 * font.pixelSize, 0.9 * maxContentWidth)

    function centerDialog() {
        // Don't bind x and y because that can cause a binding loop if the
        // application window is interactively resized
        if (ApplicationWindow.window) {
            x = (ApplicationWindow.window.width - width) / 2
            y = (ApplicationWindow.window.height - height) / 2
        }
    }
    // Qt 5.11 doesn't support default buttons yet, this function can be
    // called as a replacement if the Return key is pressed and should be
    // reimplemented if needed in derived dialogs.
    // We don't handle the return key inside the dialog because the dialog will
    // not consume the event in Qt 5.11 even if it accepts the key event and
    // might therefore trigger global actions.
    function returnPressed() {
        if (! hasButtonFocus())
            accept()
    }
    // Check if any button in the footer the focus. We don't want to handle
    // the return key as accept (see comemnt above) if any button has the
    // visual focus because the user might expect that pressing return triggers
    // the button with the focus.
    function hasButtonFocus() {
        for (var i = 0; i < footer.contentChildren.length; ++i) {
            if (footer.contentChildren[i].visualFocus)
                return true
        }
        return false
    }

    // We make all dialogs modal even if they wouldn't need to be because
    // QtQuickControls2 dialogs are not windows but immovable popup items, so
    // they inevitably cover parts of the parent window, such that the parent
    // window is not fully usable anyway.
    modal: true
    dim: false

    focus: true
    clip: true
    closePolicy: Popup.CloseOnEscape
    onAboutToShow: centerDialog()
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

    Shortcut {
        sequence: "Return"
        enabled: visible
        onActivated: returnPressed()
    }
}
