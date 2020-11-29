import QtQuick 2.11
import QtQuick.Controls 2.2

PentobiDialog {
    id: root

    footer: PentobiDialogButtonBox { ButtonClose { } }

    Item {
        implicitWidth:
            Math.max(Math.min(column.implicitWidth, maxContentWidth),
                     minContentWidth)
        implicitHeight: column.implicitHeight

        Column {
            id: column

            anchors.fill: parent
            spacing: 0.5 * font.pixelSize
            leftPadding: spacing
            rightPadding: leftPadding

            Image {
                source: "qrc:icon/pentobi-128.png"
                width: 64; height: 64
                sourceSize { width: 256; height: 256 }
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                //: The argument is the application version.
                text: qsTr("Pentobi %1").arg(Qt.application.version)
                font {
                    bold: true
                    pixelSize: 1.3 * root.font.pixelSize
                }
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                text: qsTr("Computer opponent for the board game Blokus")
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                width: Math.min(implicitWidth, maxContentWidth)
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label {
                text: "<a href=\"https://pentobi.sourceforge.io\" style=\"text-decoration:none\">pentobi.sourceforge.io</a>"
                textFormat: Text.RichText
                elide: Qt.ElideRight
                width: Math.min(implicitWidth, maxContentWidth)
                anchors.horizontalCenter: parent.horizontalCenter
                onLinkActivated: Qt.openUrlExternally(link)

                MouseArea {
                    enabled: isDesktop
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.NoButton
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }
            Label {
                text: qsTr("Copyright © 2011–%1 Markus Enzenberger").arg(2020)
                font.pixelSize: 0.9 * root.font.pixelSize
                opacity: 0.8
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                width: Math.min(implicitWidth, maxContentWidth)
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
