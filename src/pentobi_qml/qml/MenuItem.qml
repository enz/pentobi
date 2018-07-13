import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "Main.js" as Logic

MenuItem {
    id: root

    // implicitWidth should be the same as in Pentobi.Menu
    implicitWidth: Math.min(font.pixelSize * (isDesktop ? 21 : 18), rootWindow.width)
    implicitHeight: (isDesktop ? 2 : 2.2) * font.pixelSize
    background: Rectangle {
        x: 1; y: 1
        width: root.width - 2; height: root.height - 2
        color: root.highlighted ? "#f0f0f0" : "transparent"
    }
    contentItem: RowLayout {
        opacity: enabled ? 1.0 : 0.3

        Item { implicitWidth: root.indicator.width }
        Text {
            text:  {
                if (isAndroid)
                    return root.text.replace("&", "")
                var pos = root.text.indexOf("&")
                if (pos < 0 || pos === root.text.length - 1)
                    return
                return root.text.substring(0, pos) + "<u>"
                        + root.text.substring(pos + 1, pos + 2)
                        + "</u>" + root.text.substring(pos + 2)
            }
            font: root.font
            elide: Text.ElideRight
            color: "black"
            Layout.fillWidth: true
        }
        Text {
            visible: isDesktop
            text: {
                if (! root.action || ! root.action.shortcut)
                    return ""
                var text = root.action.shortcut
                text = text.replace("Ctrl",
                                    ///: Shortcut modifier key as displayed in menu item text (abbreviate if long)
                                    qsTr("Ctrl"))
                return text.replace("Shift",
                                    ///: Shortcut modifier key as displayed in menu item text (abbreviate if long)
                                    qsTr("Shift"))
            }
            font: root.font
            color: "#666"
        }
        Item { implicitWidth: root.arrow.width }
    }
}
