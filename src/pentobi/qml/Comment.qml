//-----------------------------------------------------------------------------
/** @file pentobi/qml/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2

ScrollView {
    function dropFocus() { if (textArea.activeFocus) textArea.focus = false }

    clip: true
    ScrollBar.vertical.minimumSize: 0.2

    TextArea {
        id: textArea

        text: gameModel.comment
        color: theme.colorCommentText
        selectionColor: theme.colorSelection
        selectedTextColor: theme.colorSelectedText
        selectByMouse: isDesktop
        wrapMode: TextEdit.Wrap
        focus: true
        onTextChanged: gameModel.comment = text
        background: Rectangle {
            // Qt 5.12.0 alpha doesn't size the background if it is in a
            // SwipeView like in GameDisplayMobile
            anchors.fill: parent
            color: theme.colorCommentBase
            radius: 2
            border.color:
                textArea.activeFocus ? theme.colorCommentFocus
                                     : theme.colorCommentBorder
        }
        Keys.onPressed:
            if (event.key === Qt.Key_Tab)
            {
                focus = false
                event.accepted = true
            }
    }
}
