//-----------------------------------------------------------------------------
/** @file pentobi/qml/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2

Rectangle {
    function dropFocus() { textArea.focus = false }

    color: theme.colorCommentBase
    radius: 2
    border.color:
        textArea.activeFocus ? theme.colorCommentFocus
                             : theme.colorCommentBorder

    ScrollView {
        anchors.fill: parent
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
            Keys.onPressed:
                if (event.key === Qt.Key_Tab) {
                    focus = false
                    event.accepted = true
                }
        }
    }
}
