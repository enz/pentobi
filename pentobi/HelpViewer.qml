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

    contentWidth: width
    contentHeight: textArea.implicitHeight

    Item
    {
        width: Math.max(parent.width - 12, 0)

        TextArea {
            id: textArea

            width: Math.min(parent.width, 800)
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            textFormat: TextArea.RichText
            wrapMode: TextArea.WordWrap
            readOnly: true
            hoverEnabled: ! isMobile
            activeFocusOnPress: false // See also comment at workaroundOskBug
            text: docbookReader.text
            // Selecting text sometimes triggers Qt bugs that break
            // clicking on links (Qt 6.9.2)
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
                visible: ! isMobile
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
        textWidth: textArea.width
    }
}
