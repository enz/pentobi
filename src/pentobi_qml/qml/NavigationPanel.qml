import QtQuick 2.0
import QtQuick.Layouts 1.0

ColumnLayout {
    id: root

    property alias activeFocusComment: comment.activeFocus

    Comment {
        id: comment

        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Text {
        id: positionInfo

        text: gameModel.positionInfo
        color: theme.fontColorPosInfo
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    }
    NavigationButtons {
        width: root.width; height: width / 6
    }
}
