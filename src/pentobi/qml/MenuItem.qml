//-----------------------------------------------------------------------------
/** @file pentobi/qml/MenuItem.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import "Controls.js" as PentobiControls

// Custom menu item that displays shortcuts (MenuItem in Qt 5.11 does not).
MenuItem {
    id: root

    property string shortcut: action && action.shortcut ? action.shortcut : ""

    function addMnemonic(text, mnemonic) {
        return PentobiControls.addMnemonic(text, mnemonic)
    }

    property real _anyItemIndicatorWidth: {
        if (menu)
            for (var i = 0; i < menu.count; ++i)
                if (menu.itemAt(i).checkable)
                    return menu.itemAt(i).indicator.width
        return 0
    }
    property real _anyItemArrowWidth: {
        if (menu)
            for (var i = 0; i < menu.count; ++i)
                if (menu.menuAt(i))
                    return menu.itemAt(i).arrow.width
        return 0
    }

    // Qt 5.12.0 alpha doesn't set the width of menu items
    width: menu.width
    implicitHeight:
        Math.round(font.pixelSize * (isDesktop ? 1.9 : 2.2)
                   * Screen.devicePixelRatio) / Screen.devicePixelRatio
    // Explicitly set hoverEnabled to true, otherwise hover highlighting and
    // submenu opening doesn't work in KDE on Ubuntu 18.10 (bug in Qt?)
    hoverEnabled: true
    Keys.onPressed:
        // Workaround for QTBUG-70181 (disabled items take part in arrow key
        // navigation). Only handle Up key, the Down key case is already
        // handled in Pentobi.Menu.onCurrentIndexChanged
        if (event.key === Qt.Key_Up && menu) {
            for (var i = menu.currentIndex - 1; i >= 0; --i)
                if (menu.itemAt(i) instanceof MenuItem && menu.itemAt(i).enabled) {
                    menu.currentIndex = i
                    break
                }
            event.accepted = true
        }
    background: Rectangle {
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
            color: {
                // See comment at background
                if (root.highlighted)
                    return isDesktop ? palette.highlightedText : palette.buttonText
                return palette.text
            }
            Layout.fillWidth: true
            Layout.leftMargin: 0.1 * font.pixelSize + _anyItemIndicatorWidth
                               + 0.2 * font.pixelSize
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
            Layout.rightMargin: _anyItemArrowWidth > 0 ? _anyItemArrowWidth
                                                       : 0.1 * font.pixelSize
        }
    }
}
