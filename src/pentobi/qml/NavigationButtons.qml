//-----------------------------------------------------------------------------
/** @file pentobi/qml/NavigationButtons.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0
import "." as Pentobi

RowLayout
{
    spacing: 0

    Pentobi.Button {
        id: buttonBeginning

        icon.source: theme.getImage("pentobi-beginning")
        action: actions.beginning
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-backward")
        action: actions.backward
        autoRepeat: true
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-forward")
        action: actions.forward
        autoRepeat: true
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-end")
        action: actions.end
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-previous-variation")
        action: actions.prevVar
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Pentobi.Button {
        icon.source: theme.getImage("pentobi-next-variation")
        action: actions.nextVar
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
