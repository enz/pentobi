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
        id: scrollView

        anchors.fill: parent
        ScrollBar.vertical.minimumSize: 0.15
        // Workaround for QTBUG-140033 (Scrollbar not painted in Fusion style,
        // Qt 6.9.2)
        ScrollBar.vertical.contentItem: Rectangle {
            implicitWidth: 6
            radius: 3
            color: theme.colorText
            opacity:
                if (scrollView.ScrollBar.vertical.pressed)
                    return 0.4
                else if (scrollView.ScrollBar.vertical.hovered)
                    return 0.3
                else if (scrollView.ScrollBar.vertical.size < 1)
                    return 0.2
                else
                    return 0
        }

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
