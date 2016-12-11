import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import "." as Pentobi

ColumnLayout {
    id: root

    property alias positionInfoSize: positionInfo.font.pixelSize

    TextArea {
        Layout.fillWidth: true
        Layout.fillHeight: true
        style: TextAreaStyle {
            textColor: theme.fontColorPosInfo
            selectionColor: theme.selectionColor
            selectedTextColor: theme.selectedTextColor
            backgroundColor: theme.backgroundColor
        }

        text: gameModel.comment
        onTextChanged: gameModel.comment = text
    }
    Text {
        id: positionInfo

        text: gameModel.positionInfo
        color: theme.fontColorPosInfo
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    }
    RowLayout
    {
        width: root.width; height: width / 6

        Pentobi.Button {
            enabled: gameModel.canGoBackward
            imageSource: "icons/pentobi-beginning.svg"
            Layout.fillWidth: true
            onClicked: gameModel.goBeginning()
        }
        Pentobi.Button {
            enabled: gameModel.canGoBackward
            imageSource: "icons/pentobi-backward.svg"
            Layout.fillWidth: true
            onClicked: gameModel.goBackward()
            autoRepeat: true
        }
        Pentobi.Button {
            enabled: gameModel.canGoForward
            imageSource: "icons/pentobi-forward.svg"
            Layout.fillWidth: true
            onClicked: gameModel.goForward()
            autoRepeat: true
        }
        Pentobi.Button {
            enabled: gameModel.canGoForward
            imageSource: "icons/pentobi-end.svg"
            Layout.fillWidth: true
            onClicked: gameModel.goEnd()
        }
        Pentobi.Button {
            enabled: gameModel.hasPrevVar
            imageSource: "icons/pentobi-previous-variation.svg"
            Layout.fillWidth: true
            onClicked: gameModel.goPrevVar()
        }
        Pentobi.Button {
            enabled: gameModel.hasNextVar
            imageSource: "icons/pentobi-next-variation.svg"
            Layout.fillWidth: true
            onClicked: gameModel.goNextVar()
        }
    }
}
