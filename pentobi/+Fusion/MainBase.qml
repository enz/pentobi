//-----------------------------------------------------------------------------
/** @file pentobi/+Fusion/MainBase.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

// Fusion is broken on some platforms and uses a light palette even when
// colorScheme is dark (#26; QTBUG-148413). We hardcode the values from
// qtbase/src/gui/kernel/qplatformtheme.cpp (Qt 6.11.3) to ensure the palette
// matches colorScheme. Note that the value of colorScheme depends on the
// platform theme plugin and is broken on some platforms.
ApplicationWindow {
    property bool isDark: Application.styleHints.colorScheme === Qt.Dark

    QtObject {
        id: _d

        property color alternateBase: mix(base, button)
        property color brightText: isDark ? "black" : "white"
        property color button: window
        property color base: isDark ? Qt.darker(window, 1.4) : "white"
        property color dark: Qt.darker(window, 1.5)
        property color highlight: "#308cc6"
        property color highlightedText: isDark ? windowText : "white"
        property color light: Qt.lighter(window, 1.5)
        property color link: isDark ? highlight : "blue"
        property color linkVisited: "magenta"
        property color mid: Qt.darker(window, 1.3)
        property color midlight: Qt.lighter(mid, 1.1)
        property color shadow: Qt.darker(dark, 1.35)
        property color text: isDark ? windowText : "black"
        property color placeHolderText: Qt.alpha(text, 0.5)
        property color toolTipBase: "#ffffdc"
        property color toolTipText: "black"
        property color window: isDark ? "#323232" : "#efefef"
        property color windowText: isDark ? "#f0f0f0" : "black"
        property color disabledBase: window
        property color disabledDark: Qt.darker("#d1d1d1", 1.1)
        property color disabledHighlight: "#919191"
        property color disabledShadow: Qt.lighter(shadow, 1.5)
        property color disabledText: isDark ? "#828282" : "#bebebe"

        function mix(a, b) {
            return Qt.rgba((a.r + b.r) / 2, (a.g + b.g) / 2, (a.b + b.b) / 2,
                           (a.a + b.a) / 2)
        }
    }

    palette {
        active {
            accent: _d.highlight
            alternateBase: _d.alternateBase
            base: _d.base
            brightText: _d.brightText
            button: _d.button
            buttonText: _d.text
            dark: _d.dark
            highlight: _d.highlight
            highlightedText: _d.highlightedText
            light: _d.light
            link: _d.link
            linkVisited: _d.linkVisited
            mid: _d.mid
            midlight: _d.midlight
            shadow: _d.shadow
            text: _d.text
            placeholderText: _d.placeholderText
            toolTipBase: _d.toolTipBase
            toolTipText: _d.toolTipText
            window: _d.window
            windowText: _d.windowText
        }
        disabled {
            accent: _d.highlight
            alternateBase: _d.alternateBase
            base: _d.disabledBase
            brightText: _d.brightText
            button: _d.button
            buttonText: _d.disabledText
            dark: _d.disabledDark
            highlight: _d.disabledHighlight
            highlightedText: _d.highlightedText
            light: _d.light
            link: _d.link
            linkVisited: _d.linkVisited
            mid: _d.mid
            midlight: _d.midlight
            shadow: _d.disabledShadow
            text: _d.disabledText
            placeholderText: _d.placeholderText
            toolTipBase: _d.toolTipBase
            toolTipText: _d.toolTipText
            window: _d.window
            windowText: _d.disabledText
        }
        inactive {
            accent: _d.highlight
            alternateBase: _d.alternateBase
            base: _d.base
            brightText: _d.brightText
            button: _d.button
            buttonText: _d.text
            dark: _d.dark
            highlight: _d.highlight
            highlightedText: _d.highlightedText
            light: _d.light
            link: _d.link
            linkVisited: _d.linkVisited
            mid: _d.mid
            midlight: _d.midlight
            shadow: _d.shadow
            text: _d.text
            placeholderText: _d.placeholderText
            toolTipBase: _d.toolTipBase
            toolTipText: _d.toolTipText
            window: _d.window
            windowText: _d.windowText
        }
    }
}
