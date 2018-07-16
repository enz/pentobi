import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "Controls.js" as PentobiControls

MenuItem {
    id: root

    // See PentobiControls.addMnemonic()
    function addMnemonic(text, mnemonic) { return PentobiControls.addMnemonic(text, mnemonic) }

    // implicitWidth should be the same as in Pentobi.Menu
    implicitWidth: Math.min(font.pixelSize * (isDesktop ? 22 : 18), rootWindow.width)
    implicitHeight: (isDesktop ? 2 : 2.2) * font.pixelSize
    background: Rectangle {
        x: 1; y: 1
        width: root.width - 2; height: root.height - 2
        color: root.highlighted ? "#e0e0e0" : "transparent"
    }
    indicator: Item {
        implicitWidth: 1.5 * font.pixelSize; implicitHeight: implicitWidth
        anchors.verticalCenter: parent.verticalCenter

        Image {
            width: 1.2 * font.pixelSize; height: width
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            visible: root.checked
            source: "icons/menu-indicator.svg"
            sourceSize { width: width; height: height }
        }
    }
    contentItem: RowLayout {
        spacing: 0
        opacity: enabled ? 1.0 : 0.3
        anchors.fill: parent

        Item { implicitWidth: root.indicator.width }
        Text {
            text: {
                if (isAndroid)
                    return root.text.replace("&", "")
                var pos = root.text.indexOf("&")
                if (pos < 0 || pos === root.text.length - 1)
                    return root.text
                return root.text.substring(0, pos) + "<u>"
                        + root.text.substring(pos + 1, pos + 2)
                        + "</u>" + root.text.substring(pos + 2)
            }
            font: root.font
            elide: Text.ElideRight
            color: "black"
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }
        Text {
            visible: isDesktop
            text: {
                if (! root.action || ! root.action.shortcut)
                    return ""
                var text = root.action.shortcut
                //: Shortcut modifier key as displayed in menu item text (abbreviate if long)
                text = text.replace("Ctrl", qsTr("Ctrl"))
                //: Shortcut modifier key as displayed in menu item text (abbreviate if long)
                return text.replace("Shift", qsTr("Shift"))
            }
            font: root.font
            color: "#666"
            Layout.alignment: Qt.AlignVCenter
        }
        Item { implicitWidth: root.arrow.width + 3 }
    }
}
