import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import "." as Pentobi

Column {
    id: root

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
        }
        Pentobi.Button {
            enabled: gameModel.canGoForward
            imageSource: "icons/pentobi-forward.svg"
            Layout.fillWidth: true
            onClicked: gameModel.goForward()
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
    Text {
        anchors.horizontalCenter: root.horizontalCenter
        text: gameModel.positionInfo
        color: theme.fontColorPosInfo
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenterCenter
    }
}
