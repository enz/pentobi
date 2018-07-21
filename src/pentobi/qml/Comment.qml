//-----------------------------------------------------------------------------
/** @file pentobi/qml/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2

ScrollView {
    clip: true

    TextArea {
        id: textArea

        text: gameModel.comment
        onTextChanged: gameModel.comment = text
        color: theme.colorText
        selectionColor: theme.colorSelection
        selectedTextColor: theme.colorSelectedText
        selectByMouse: isDesktop
        wrapMode: TextEdit.Wrap
        background: Rectangle {
            color: theme.colorBackground
            border.color: textArea.activeFocus ?
                              theme.colorCommentFocus : theme.colorCommentBorder
        }
        Keys.onPressed:
            if (event.key === Qt.Key_Tab)
            {
                focus = false
                event.accepted = true
            }
    }
}
