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
        iconSource: "pentobi-beginning"
        action: actionBeginning
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        iconSource: "pentobi-backward"
        action: actionBackward
        autoRepeat: true
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        iconSource: "pentobi-forward"
        action: actionForward
        autoRepeat: true
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        iconSource: "pentobi-end"
        action: actionEnd
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        iconSource: "pentobi-previous-variation"
        action: actionPrevVar
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    PentobiButton {
        iconSource: "pentobi-next-variation"
        action: actionNextVar
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
}
