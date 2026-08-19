//-----------------------------------------------------------------------------
/** @file pentobi/HelpViewer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
// Enforce Basic style to avoid frame around TextArea in some styles
import QtQuick.Controls.Basic

PentobiScrollView {
    id: root

    property bool isDark

    signal closeClicked()

    rightPadding: 6

    // Without declaring a Flickable inside the ScrollView, the TextArea can
    // needlessly be flicked horizontally on Android (Qt 6.10)
    Flickable {
        contentWidth: width
        contentHeight: textArea.implicitHeight
        clip: true

        TextArea {
            id: textArea

            width: parent.width
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
    }
    DocbookReader {
        id: docbookReader

        isDark: root.isDark
        colorBackground: theme.colorBackground
        colorText: theme.colorText
        colorLink: theme.colorLink
        textWidth: parent.width - textArea.leftPadding - textArea.rightPadding
    }
}
