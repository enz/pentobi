//-----------------------------------------------------------------------------
/** @file pentobi/+Basic/CommentBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

/** Workaround for QTBUG-149233 (TextArea background invisible in Basic
    style). */
Rectangle {
    property bool drawFocus

    color: "transparent"
    border.width: 1
    border.color: {
        if (isDark)
            return drawFocus ? "#0D69F2" : "#626262"
        else
            return drawFocus ? "#0066FF" : "#BDBDBD"
    }
    radius: 2
}
