//-----------------------------------------------------------------------------
/** @file pentobi/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

ScrollView {
    property alias hasFocus: textArea.activeFocus

    function dropFocus() { textArea.focus = false }

    clip: true
    ScrollBar.vertical.minimumSize: 0.2

    TextArea {
        id: textArea

        text: gameModel.comment
        placeholderText: qsTr("Comment")
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
