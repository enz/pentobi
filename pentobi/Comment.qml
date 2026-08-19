//-----------------------------------------------------------------------------
/** @file pentobi/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

CommentBase {
    property alias hasFocus: textArea.activeFocus

    function dropFocus() { textArea.focus = false }

    drawFocus: textArea.activeFocus

    PentobiScrollView {
        anchors.fill: parent
        ScrollBar.vertical.minimumSize: 0.15

        TextArea {
            id: textArea

            text: gameModel.comment
            selectByMouse: isDesktop
            wrapMode: TextEdit.Wrap
            focus: true
            onTextChanged: gameModel.comment = text
            Keys.onTabPressed: focus = false
        }
    }
}
