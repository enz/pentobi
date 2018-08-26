//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuItem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import "Controls.js" as PentobiControls

// Custom menu item that displays shortcuts (MenuItem in Qt 5.11 does not).
MenuItem {
    id: root

    property string shortcut: action && action.shortcut ? action.shortcut : ""

    function addMnemonic(text, mnemonic) {
        return PentobiControls.addMnemonic(text, mnemonic)
    }

    property bool _hasMenuIndicators: {
        if (! menu)
            return false
        for (var i = 0; i < menu.count; ++i)
            if (menu.itemAt(i).checkable)
                return true
        return false
    }
    property bool _hasMenuArrows: {
        if (! menu)
            return false
        for (var i = 0; i < menu.count; ++i)
            if (menu.menuAt(i))
                return true
        return false
    }

    height: font.pixelSize * (isDesktop ? 1.9 : 2.2)
    // Explicitely set hoverEnabled to true, otherwise hover highlighting and
    // submenu opening doesn't work in KDE on Ubuntu 18.10 (bug in Qt?)
    hoverEnabled: true
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
        anchors.fill: parent

        Item {
            implicitWidth: {
                var result = 0.1 * font.pixelSize
                if (_hasMenuIndicators)
                    result += root.indicator.width + 0.2 * font.pixelSize
                return result
            }
        }
        Label {
            id: labelText

            text: {
                if (! isDesktop)
                    return root.text
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
            visible: isDesktop && shortcut !== ""
            text: {
                var text = shortcut
                //: Shortcut modifier key as displayed in menu item text (abbreviate if long)
                text = text.replace("Ctrl", qsTr("Ctrl"))
                //: Shortcut modifier key as displayed in menu item text (abbreviate if long)
                return text.replace("Shift", qsTr("Shift"))
            }
            color: labelText.color
            opacity: 0.6
            Layout.alignment: Qt.AlignVCenter
        }
        Item {
            implicitWidth:
                _hasMenuArrows ? root.arrow.width : 0.1 * font.pixelSize
        }
    }
}
