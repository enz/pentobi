//-----------------------------------------------------------------------------
/** @file pentobi/qml/ButtonToolTip.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import QtQuick.Controls 2.4
import "." as Pentobi

// Used instead of attached tooltip because of QTBUG-30801 (tooltip not shown
// when the button is disabled). Must be declared such that x and y have the
// same meaning as in the button (e.g. same parent).
MouseArea {
    id: root

    property Pentobi.Button button

    visible: button.visible && isDesktop
    x: button.x
    y: button.y
    width: button.width
    height: button.height
    acceptedButtons: Qt.NoButton
    hoverEnabled: true
    ToolTip.text: button.ToolTip.text
    ToolTip.visible: containsMouse
    ToolTip.delay: 1000
    ToolTip.timeout: 7000
    Component.onCompleted:
        button.buttonToolTipHovered = Qt.binding(function() {
            return containsMouse })

    Connections {
        target: button
        onPressed: root.ToolTip.visible = false
    }
}
