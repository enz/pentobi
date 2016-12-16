import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import "." as Pentobi

ColumnLayout {
    id: root

    TextArea {
        Layout.fillWidth: true
        Layout.fillHeight: true
        style: TextAreaStyle {
            textColor: theme.fontColorPosInfo
            selectionColor: theme.selectionColor
            selectedTextColor: theme.selectedTextColor
            backgroundColor: theme.backgroundColor
        }
        // Selection doesn't work very well on Android with Qt 5.8
        selectByKeyboard: ! isAndroid
        selectByMouse: ! isAndroid
        text: gameModel.comment
        onTextChanged: gameModel.comment = text
    }
    Text {
        id: positionInfo

        text: gameModel.positionInfo
        color: theme.fontColorPosInfo
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    }
    NavigationButtons
    {
        width: root.width; height: width / 6
    }
}
