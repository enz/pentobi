//-----------------------------------------------------------------------------
/** @file pentobi/qml/NavigationButtons.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

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

        icon.source: theme.getImage("pentobi-beginning")
        width: buttonWidth; height: buttonHeight
        action: actions.beginning
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-backward")
        width: buttonWidth; height: buttonHeight
        action: actions.backward
        autoRepeat: true
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-forward")
        width: buttonWidth; height: buttonHeight
        action: actions.forward
        autoRepeat: true
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-end")
        width: buttonWidth; height: buttonHeight
        action: actions.end
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-previous-variation")
        width: buttonWidth; height: buttonHeight
        action: actions.prevVar
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-next-variation")
        width: buttonWidth; height: buttonHeight
        action: actions.nextVar
    }
}
