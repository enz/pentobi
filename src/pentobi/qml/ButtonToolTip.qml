//-----------------------------------------------------------------------------
/** @file pentobi/qml/ButtonToolTip.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import QtQuick.Controls 2.4

// Used instead of attached tooltip because of QTBUG-30801 (tooltip not shown
// when the button is disabled). Must be declared such that x and y have the
// same meaning as in the button (e.g. same parent).
MouseArea {
    property AbstractButton button

    visible: button.visible && isDesktop
    x: button.x
    y: button.y
    width: button.width
    height: button.height
    acceptedButtons: Qt.NoButton
    hoverEnabled: true
    ToolTip.text: button.ToolTip.text
    ToolTip.delay: 1000
    ToolTip.timeout: 7000
    onEntered: ToolTip.visible = true
    onExited: ToolTip.visible = false

    Connections {
        target: button
        onPressed: parent.ToolTip.visible = false
    }
}
