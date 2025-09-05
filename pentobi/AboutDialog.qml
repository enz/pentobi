//-----------------------------------------------------------------------------
/** @file pentobi/AboutDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

PentobiDialog {
    id: root

    footer: PentobiDialogButtonBox { ButtonClose { } }

    Item {
        implicitWidth:
            Math.max(Math.min(column.implicitWidth, root.maxContentWidth),
                     root.minContentWidth)
        implicitHeight: column.implicitHeight

        Column {
            id: column

            anchors.fill: parent
            spacing: 0.5 * root.font.pixelSize
            leftPadding: spacing
            rightPadding: leftPadding

            Image {
                source: "icon/pentobi-48.png"
                width: 48; height: 48
                sourceSize { width: 192; height: 192 }
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                //: The argument is the application version.
                text: "<h3>" + qsTr("Pentobi %1").arg(Qt.application.version)
                font { bold: true }
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                text: qsTr("Computer opponent for Blokus")
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                width: Math.min(implicitWidth, root.maxContentWidth)
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                text:
                    // Default link color in dark mode has low contrast (Qt 6.9)
                    "<style>:link{text-decoration:none;color:\""
                    + (theme.isDark ? "lightskyblue" : "blue")
                    + "\"}</style><a href=\"https://pentobi.sourceforge.io\">pentobi.sourceforge.io</a>"
                textFormat: Text.RichText
                elide: Qt.ElideRight
                width: Math.min(implicitWidth, root.maxContentWidth)
                anchors.horizontalCenter: parent.horizontalCenter
                onLinkActivated: link => Qt.openUrlExternally(link)

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.NoButton
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
            Label {
                text: qsTr("Copyright © 2011–%1 Markus Enzenberger").arg(2025)
                font.pixelSize: 0.9 * root.font.pixelSize
                opacity: 0.8
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                width: Math.min(implicitWidth, root.maxContentWidth)
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
