//-----------------------------------------------------------------------------
/** @file pentobi/CommentBackground.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

/** Needed to work around a TextArea bug in Basic style.
    See +Basic/CommentBackground.qml */
Item {
    property bool drawFocus
}
