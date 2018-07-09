import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "Main.js" as Logic

MenuItem {
    id: root

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
            elide: Text.ElideRight
            Layout.fillWidth: true
        }
        Text {
            visible: isDesktop
            text: {
                if (! root.action || ! root.action.shortcut)
                    return ""
                var text = root.action.shortcut
                ///: Shortcut modifier key as displayed in menu item text
                text = text.replace("Ctrl", qsTr("Ctrl"))
                ///: Shortcut modifier key as displayed in menu item text
                return text.replace("Shift", qsTr("Shift"))
            }
            color: "#666"
        }
        Item { implicitWidth: root.arrow.width }
    }
}
