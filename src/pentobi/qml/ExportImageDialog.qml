import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import "." as Pentobi

Pentobi.Dialog {
    title: isDesktop ? qsTr("Export Image") : ""
    footer: OkCancelButtons { }
    onAccepted: {
        exportImageWidth = parseInt(textField.text)
        imageSaveDialog.open()
    }

    Item {
        implicitWidth: {
            var w = rowLayout.implicitWidth
            // Avoid too small width because Default style in Qt 5.11 makes
            // footer no wider than content, which can cause elided text on
            // dialog buttons
            w = Math.max(w, font.pixelSize * 18)
            w = Math.min(w, 0.85 * rootWindow.width)
            return w
        }
        implicitHeight: rowLayout.implicitHeight

        RowLayout {
            id: rowLayout

            anchors.fill: parent

            Label { text: qsTr("Image width:") }
            TextField {
                id: textField

                text: exportImageWidth
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator{ bottom: 0; top: 32767 }
                Layout.preferredWidth: font.pixelSize * 7
            }
            Item { Layout.fillWidth: true }
        }
    }
}
