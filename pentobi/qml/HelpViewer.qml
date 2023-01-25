//-----------------------------------------------------------------------------
/** @file pentobi/qml/HelpViewer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import pentobi

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
        if (docbookReader.nextPageId !== "")
            docbookReader.pageId = docbookReader.nextPageId
    }
    function prevPage() {
        if (docbookReader.prevPageId !== "")
            docbookReader.pageId = docbookReader.prevPageId
    }

    color: "white"

    DocbookReader {
        id: docbookReader

        textWidth: textArea.width - 2 * textAreaPadding
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TextArea {
            id: navigation

            Layout.fillWidth: true
            // Avoid frame in some styles
            // (https://github.com/lwchkg/pentobi/issues/2#issuecomment-1095506572)
            background: Item { }
            leftPadding: font.pixelSize
            rightPadding: leftPadding
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
            Layout.fillWidth: true
            orientation: Qt.Horizontal
        }
        Flickable {
            id: flickable

            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true
            boundsMovement: Flickable.StopAtBounds
            contentWidth: width
            contentHeight: textArea.implicitHeight

            TextArea {
                id: textArea

                width: flickable.width
                // Avoid frame in some styles
                // (https://github.com/lwchkg/pentobi/issues/2#issuecomment-1095506572)
                background: Item { }
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
            ScrollBar.vertical: ScrollBar {
                policy: isDesktop ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
            }
        }
    }
}
