import QtQuick 2.0
import QtQuick.Controls 2.2

ScrollView {
    TextArea {
        text: gameModel.comment
        onTextChanged: gameModel.comment = text
        color: theme.fontColorPosInfo
        wrapMode: TextEdit.Wrap
        background: Rectangle {
            color: theme.backgroundColor
            border.color: theme.commentBorderColor
        }
        Keys.onPressed:
            if (event.key === Qt.Key_Tab)
            {
                focus = false
                event.accepted = true
            }
    }
}
