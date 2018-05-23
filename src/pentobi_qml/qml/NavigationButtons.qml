import QtQuick 2.0
import QtQuick.Layouts 1.0
import "." as Pentobi

RowLayout
{
    Pentobi.Button {
        enabled: gameModel.canGoBackward
        imageSource: theme.getImage("pentobi-beginning")
        Layout.fillWidth: true
        onClicked: gameModel.goBeginning()
    }
    Pentobi.Button {
        enabled: gameModel.canGoBackward
        imageSource: theme.getImage("pentobi-backward")
        Layout.fillWidth: true
        onClicked: gameModel.goBackward()
        autoRepeat: true
    }
    Pentobi.Button {
        enabled: gameModel.canGoForward
        imageSource: theme.getImage("pentobi-forward")
        Layout.fillWidth: true
        onClicked: gameModel.goForward()
        autoRepeat: true
    }
    Pentobi.Button {
        enabled: gameModel.canGoForward
        imageSource: theme.getImage("pentobi-end")
        Layout.fillWidth: true
        onClicked: gameModel.goEnd()
    }
    Pentobi.Button {
        enabled: gameModel.hasPrevVar
        imageSource: theme.getImage("pentobi-previous-variation")
        Layout.fillWidth: true
        onClicked: gameModel.goPrevVar()
    }
    Pentobi.Button {
        enabled: gameModel.hasNextVar
        imageSource: theme.getImage("pentobi-next-variation")
        Layout.fillWidth: true
        onClicked: gameModel.goNextVar()
    }
}
