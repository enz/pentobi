//-----------------------------------------------------------------------------
/** @file pentobi/HelpViewer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
// Enforce Basic style to avoid frames around TextArea in Material and
// Universal style
import QtQuick.Controls.Basic
import PentobiGui

Rectangle {
    property real textAreaPadding:
        Math.max((width - 45 * textArea.font.pixelSize) / 2,
                 textArea.font.pixelSize)

    function goHome() {
        docbookReader.pageId = "index"
    }
    function scrollUp() {
        if (flickable.contentY > 0)
            flickable.contentY =
                    Math.max(flickable.contentY - textArea.font.pixelSize, 0)
    }
    function scrollDown() {
        if (flickable.contentY < flickable.contentHeight - textArea.font.pixelSize)
            flickable.contentY += textArea.font.pixelSize
    }
    function scrollPageUp() {
        if (flickable.contentY > 0)
            flickable.contentY =
                    Math.max(flickable.contentY - flickable.height, 0)
    }
    function scrollPageDown() {
        if (flickable.contentY < flickable.contentHeight - flickable.height)
            flickable.contentY += flickable.height
    }
    function nextPage() {
        if (docbookReader.getNextPageId() !== "")
            docbookReader.pageId = docbookReader.getNextPageId()
    }
    function prevPage() {
        if (docbookReader.getPrevPageId() !== "")
            docbookReader.pageId = docbookReader.getPrevPageId()
    }

    color: "white"

    DocbookReader {
        id: docbookReader

        textWidth: textArea.width - 2 * textAreaPadding
    }
    Flickable {
        id: flickable

        anchors.fill: parent
        clip: true
        boundsMovement: Flickable.StopAtBounds
        contentWidth: width
        contentHeight: column.implicitHeight

        Column {
            id : column

            width: flickable.width
            spacing: 0

            TextArea {
                width: parent.width
                leftPadding: textAreaPadding
                rightPadding: textAreaPadding
                textFormat: TextArea.RichText
                readOnly: true
                selectByMouse: false
                focus: false
                activeFocusOnPress: false
                text: docbookReader.navigationText
                onLinkActivated: link => docbookReader.pageId = link

                MouseArea {
                    anchors.fill: parent
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor
                                                    : Qt.ArrowCursor
                    acceptedButtons: Qt.NoButton
                }
            }
            ToolSeparator {
                width: parent.width
                orientation: Qt.Horizontal
            }
            TextArea {
                id: textArea

                width: parent.width
                leftPadding: textAreaPadding
                rightPadding: textAreaPadding
                textFormat: TextArea.RichText
                wrapMode: TextArea.WordWrap
                readOnly: true
                focus: false
                activeFocusOnPress: false
                text: docbookReader.text
                onLinkActivated: link => docbookReader.pageId = link
                onTextChanged: flickable.contentY = 0

                MouseArea {
                    anchors.fill: parent
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor
                                                    : Qt.ArrowCursor
                    acceptedButtons: Qt.NoButton
                }
            }
        }
        ScrollBar.vertical: ScrollBar {
            policy: isDesktop ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
        }
    }
}
