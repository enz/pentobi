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
    signal closeClicked()

    TextArea {
        id: textArea

        textFormat: TextArea.RichText
        wrapMode: TextArea.WordWrap
        readOnly: true
        text: docbookReader.text
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
