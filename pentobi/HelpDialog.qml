//-----------------------------------------------------------------------------
/** @file pentobi/HelpDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

// Enforce Basic style to avoid rounded dialog corners in Material style
import QtQuick.Controls.Basic

// Dialog used instead of a window for showing help on Android
Dialog {
    width: rootWindow.contentItem.width
    height: rootWindow.contentItem.height
    padding: 0
    modal: true

    // We might want to keep the current page if viewer was only temporarily
    // hidden, but reloading the index page avoid a bug that sometimes results
    // in empty text area in such situations (Qt 6.9, Android)
    onVisibleChanged:
        if (visible)
            helpViewer.loadIndex()

    HelpViewer {
        id: helpViewer

        anchors.fill: parent
        onCloseClicked: close()
    }
}
