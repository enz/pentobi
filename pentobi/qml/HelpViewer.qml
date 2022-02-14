//-----------------------------------------------------------------------------
/** @file pentobi/qml/HelpViewer.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import pentobi 1.0

Item {
    id: root

    property real textAreaPadding:
        Math.max((width - 45 * textArea.font.pixelSize) / 2,
                 textArea.font.pixelSize)

    DocbookReader {
        id: docbookReader

        textWidth: textArea.width - 2 * textAreaPadding
    }
    Rectangle {
        anchors.fill: parent
        color: "white"
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TextArea {
            id: navigation

            Layout.fillWidth: true
            leftPadding: font.pixelSize
            rightPadding: leftPadding
            textFormat: TextArea.RichText
            readOnly: true
            text: docbookReader.navigationText
            onLinkActivated: link => docbookReader.pageId = link
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
                font.pixelSize: 1.1 * navigation.font.pixelSize
                leftPadding: textAreaPadding
                rightPadding: textAreaPadding
                textFormat: TextArea.RichText
                wrapMode: TextArea.WordWrap
                readOnly: true
                text: docbookReader.text
                onLinkActivated: link => docbookReader.pageId = link
                onTextChanged: flickable.contentY = 0
            }
            ScrollBar.vertical: ScrollBar {
                policy: isDesktop ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
            }
        }
    }
    Shortcut {
        sequence: StandardKey.MoveToStartOfLine
        onActivated: docbookReader.pageId = "index"
    }
    Shortcut {
        sequence: StandardKey.MoveToNextPage
        onActivated:
            if (flickable.contentY < flickable.contentHeight - flickable.height)
                flickable.contentY += flickable.height
    }
    Shortcut {
        sequence: StandardKey.MoveToPreviousPage
        onActivated:
            if (flickable.contentY > 0)
                flickable.contentY =
                        Math.max(flickable.contentY - flickable.height, 0)
    }
    Shortcut {
        sequence: StandardKey.MoveToNextLine
        onActivated:
            if (flickable.contentY < flickable.contentHeight - textArea.font.pixelSize)
                flickable.contentY += textArea.font.pixelSize
    }
    Shortcut {
        sequence: StandardKey.MoveToPreviousLine
        onActivated:
            if (flickable.contentY > 0)
                flickable.contentY =
                        Math.max(flickable.contentY - textArea.font.pixelSize, 0)
    }
    Shortcut {
        sequence: StandardKey.MoveToNextChar
        onActivated:
            if (docbookReader.nextPageId !== "")
                docbookReader.pageId = docbookReader.nextPageId
    }
    Shortcut {
        sequence: StandardKey.MoveToPreviousChar
        onActivated:
            if (docbookReader.prevPageId !== "")
                docbookReader.pageId = docbookReader.prevPageId
    }
}
