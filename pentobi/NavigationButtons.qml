//-----------------------------------------------------------------------------
/** @file pentobi/NavigationButtons.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Layouts

RowLayout
{
    spacing: 0

    PentobiButton {
        id: buttonBeginning

        source: theme.getImage("pentobi-beginning", theme.isDark)
        action: actionBeginning
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        source: theme.getImage("pentobi-backward", theme.isDark)
        action: actionBackward
        autoRepeat: true
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        source: theme.getImage("pentobi-forward", theme.isDark)
        action: actionForward
        autoRepeat: true
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        source: theme.getImage("pentobi-end", theme.isDark)
        action: actionEnd
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        source: theme.getImage("pentobi-previous-variation", theme.isDark)
        action: actionPrevVar
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        source: theme.getImage("pentobi-next-variation", theme.isDark)
        action: actionNextVar
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
