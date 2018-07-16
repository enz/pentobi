import QtQuick 2.0
import QtQuick.Controls 2.2

ScrollView {
    clip: true

    TextArea {
        id: textArea

        text: gameModel.comment
        onTextChanged: gameModel.comment = text
        color: theme.fontColorPosInfo
        selectionColor: theme.selectionColor
        selectedTextColor: theme.selectedTextColor
        selectByMouse: isDesktop
        wrapMode: TextEdit.Wrap
        background: Rectangle {
            color: theme.backgroundColor
            border.color: textArea.activeFocus ?
                              theme.commentBorderColorFocus : theme.commentBorderColor
        }
        Keys.onPressed:
            if (event.key === Qt.Key_Tab)
            {
                focus = false
                event.accepted = true
            }
    }
}
