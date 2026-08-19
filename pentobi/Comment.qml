//-----------------------------------------------------------------------------
/** @file pentobi/Comment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

Item {
    property alias hasFocus: textArea.activeFocus

    function dropFocus() { textArea.focus = false }

    ScrollView {
        anchors.fill: parent
        ScrollBar.vertical.minimumSize: 0.15

        TextArea {
            id: textArea

            text: gameModel.comment
            selectByMouse: isDesktop
            wrapMode: TextEdit.Wrap
            focus: true
            onTextChanged: gameModel.comment = text
            Keys.onTabPressed: focus = false
        }
    }
    // Workaround for QTBUG-149233 (TextArea background invisible in Basic
    // style)
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: 1
        border.color: {
            if (globalStyle != "Basic")
                return "transparent"
            if (isDark)
                return textArea.activeFocus ? "#0D69F2" : "#626262"
            else
                return textArea.activeFocus ? "#0066FF" : "#BDBDBD"
        }
        radius: 2
        z: 1
    }
}
