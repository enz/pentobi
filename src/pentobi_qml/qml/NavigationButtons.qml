import QtQuick 2.0
import QtQuick.Layouts 1.0
import "." as Pentobi

Row
{
    property alias implicitButtonWidth: buttonBeginning.implicitWidth
    property real buttonWidth: Math.min(width / 6, implicitButtonWidth)
    property real buttonHeight: Math.min(height, buttonWidth)

    spacing: (width - 6 * buttonWidth) / 5

    Pentobi.Button {
        id: buttonBeginning

        enabled: gameModel.canGoBackward
        imageSource: theme.getImage("pentobi-beginning")
        width: buttonWidth; height: buttonHeight
        onClicked: gameModel.goBeginning()
    }
    Pentobi.Button {
        enabled: gameModel.canGoBackward
        imageSource: theme.getImage("pentobi-backward")
        width: buttonWidth; height: buttonHeight
        onClicked: gameModel.goBackward()
        autoRepeat: true
    }
    Pentobi.Button {
        enabled: gameModel.canGoForward
        imageSource: theme.getImage("pentobi-forward")
        width: buttonWidth; height: buttonHeight
        onClicked: gameModel.goForward()
        autoRepeat: true
    }
    Pentobi.Button {
        enabled: gameModel.canGoForward
        imageSource: theme.getImage("pentobi-end")
        width: buttonWidth; height: buttonHeight
        onClicked: gameModel.goEnd()
    }
    Pentobi.Button {
        enabled: gameModel.hasPrevVar
        imageSource: theme.getImage("pentobi-previous-variation")
        width: buttonWidth; height: buttonHeight
        onClicked: gameModel.goPrevVar()
    }
    Pentobi.Button {
        enabled: gameModel.hasNextVar
        imageSource: theme.getImage("pentobi-next-variation")
        width: buttonWidth; height: buttonHeight
        onClicked: gameModel.goNextVar()
    }
}
