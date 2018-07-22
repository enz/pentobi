//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuItem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "Controls.js" as PentobiControls

// Custom mneu item that displays shortcuts (MenuItem in Qt 5.11 does not).
MenuItem {
    id: root

    function addMnemonic(text, mnemonic) { return PentobiControls.addMnemonic(text, mnemonic) }

    background: Rectangle {
        anchors.fill: parent
        // Don't overdraw menu background border
        anchors.margins: 1
        color: {
            if (! root.highlighted)
                return "transparent"
            // Note that MenuItem in Qt 5.11 does neither fully use the system
            // palette, nor make its actually used colors available in its own
            // palette.
            return isDesktop ? palette.highlight : palette.midlight
        }
    }
    contentItem: RowLayout {
        spacing: 0.2 * font.pixelSize
        opacity: enabled ? 1.0 : 0.5
        anchors.fill: parent

        Item { implicitWidth: root.indicator.width + 1.1 * spacing }
        Label {
            id: labelText

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
            elide: Text.ElideRight
            color: {
                // See comment at background
                if (root.highlighted)
                    return isDesktop ? palette.highlightedText : palette.buttonText
                return palette.text
            }
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }
        Label {
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
            color: labelText.color
            opacity: 0.6
            Layout.alignment: Qt.AlignVCenter
        }
        Item { implicitWidth: root.arrow.width }
    }
}
