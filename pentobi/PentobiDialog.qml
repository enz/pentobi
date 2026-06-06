//-----------------------------------------------------------------------------
/** @file pentobi/PentobiDialog.qml
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

    // We make all dialogs modal even if they wouldn't need to be because
    // QtQuickControls2 dialogs are not windows but immovable popup items, so
    // they inevitably cover parts of the parent window, such that the parent
    // window is not fully usable anyway.
    modal: true
    dim: false
    anchors.centerIn: Overlay.overlay

    focus: true
    clip: true
    closePolicy: Popup.CloseOnEscape
}
