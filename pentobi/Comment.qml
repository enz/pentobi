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

    // Workaround for QTBUG-139715 (TextArea background invisible in Fusion)
    Rectangle {
        anchors.fill: parent
        visible: globalStyle === "Fusion"
        color: Qt.lighter(palette.window, 1.2)
        border.color:
            textArea.activeFocus ? palette.highlight
                                 : Qt.alpha(theme.colorText, 0.1)
    }
    ScrollView {
        anchors.fill: parent
        clip: true
        ScrollBar.vertical.minimumSize: 0.2

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
}
