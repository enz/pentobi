//-----------------------------------------------------------------------------
/** @file pentobi/qml/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2

ScrollView {
    function dropFocus() { textArea.focus = false }

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
        onTextChanged: gameModel.comment = text
        background: Rectangle {
            anchors.centerIn: parent
            width: parent.width - 1
            height: parent.height - 1
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
