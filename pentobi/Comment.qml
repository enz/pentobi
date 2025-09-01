//-----------------------------------------------------------------------------
/** @file pentobi/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

Rectangle {
    property alias hasFocus: textArea.activeFocus

    function dropFocus() { textArea.focus = false }

    color: theme.colorCommentBase
    radius: 2
    border.color:
        textArea.activeFocus ? theme.colorCommentFocus
                             : Qt.alpha(theme.colorCommentText, 0.3)

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
            Keys.onTabPressed: focus = false
        }
    }
}
