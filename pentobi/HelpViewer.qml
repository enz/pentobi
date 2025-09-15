//-----------------------------------------------------------------------------
/** @file pentobi/HelpViewer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
// Enforce Basic style to avoid frame around TextArea in Material and
// Universal style
import QtQuick.Controls.Basic

ScrollView {
    id: scrollView

    signal closeClicked()

    // Workaround for QTBUG-140033 (Scrollbar not painted in Fusion style,
    // Qt 6.9.2)
    ScrollBar.vertical.contentItem: Rectangle {
        implicitWidth: 6
        radius: 3
        color: "black"
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
    rightPadding: 6

    TextArea {
        id: textArea

        textFormat: TextArea.RichText
        wrapMode: TextArea.WordWrap
        readOnly: true
        text: docbookReader.text
        // Selecting text sometimes triggers Qt bugs that break clicking
        // on links (Qt 6.9.2)
        selectByMouse: false
        selectByKeyboard: false
        onLinkActivated:
            link => {
                if (link === "close")
                    closeClicked()
                else
                    docbookReader.pageId = link
            }

        MouseArea {
            anchors.fill: parent
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor
                                            : Qt.ArrowCursor
            acceptedButtons: Qt.NoButton
        }
    }
    DocbookReader {
        id: docbookReader

        textWidth: parent.width - textArea.leftPadding - textArea.rightPadding
    }
}
