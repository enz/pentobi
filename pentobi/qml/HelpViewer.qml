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
}
