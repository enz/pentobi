import QtQuick 2.0
import "." as Pentobi

Row
{
    property alias implicitButtonWidth: buttonBeginning.implicitWidth
    property real buttonWidth: Math.min(width / 6, implicitButtonWidth)
    property real buttonHeight: Math.min(height, buttonWidth)

    spacing: (width - 6 * buttonWidth) / 5

    Pentobi.Button {
        id: buttonBeginning

        imageSource: theme.getImage("pentobi-beginning")
        width: buttonWidth; height: buttonHeight
        action: actions.actionBeginning
    }
    Pentobi.Button {
        imageSource: theme.getImage("pentobi-backward")
        width: buttonWidth; height: buttonHeight
        action: actions.actionBackward
        autoRepeat: true
    }
    Pentobi.Button {
        imageSource: theme.getImage("pentobi-forward")
        width: buttonWidth; height: buttonHeight
        action: actions.actionForward
        autoRepeat: true
    }
    Pentobi.Button {
        imageSource: theme.getImage("pentobi-end")
        width: buttonWidth; height: buttonHeight
        action: actions.actionEnd
    }
    Pentobi.Button {
        imageSource: theme.getImage("pentobi-previous-variation")
        width: buttonWidth; height: buttonHeight
        action: actions.actionPrevVar
    }
    Pentobi.Button {
        imageSource: theme.getImage("pentobi-next-variation")
        width: buttonWidth; height: buttonHeight
        action: actions.actionNextVar
    }
}
