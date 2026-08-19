//-----------------------------------------------------------------------------
/** @file pentobi/CommentBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

/** Style-dependent functionality of PentobiApplication.
    Use file selectors to override. */
Item {
    /** Needed for workarounds in buggy styles where we need to draw our own
        background. Bound to activeFocus of the TextArea oin Comment. */
    property bool drawFocus
}
