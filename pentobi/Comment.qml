//-----------------------------------------------------------------------------
/** @file pentobi/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

Rectangle {
    function dropFocus() { textArea.focus = false }

    radius: 2
    border.color: textArea.activeFocus ? theme.colorCommentFocus
                                       : Qt.alpha(theme.colorText, 0.3)

    ScrollView {
        anchors.fill: parent
        clip: true
        ScrollBar.vertical.minimumSize: 0.2

        TextArea {
            id: textArea

            text: gameModel.comment
            selectByMouse: isDesktop
            wrapMode: TextEdit.Wrap
            focus: true
            onTextChanged: gameModel.comment = text
            Keys.onPressed:
                event => {
                    if (event.key === Qt.Key_Tab) {
                        focus = false
                        event.accepted = true
                    }
                }
        }
    }
}
