//-----------------------------------------------------------------------------
/** @file pentobi/qml/HelpWindow.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import QtQuick.Controls 2.3
import QtWebView 1.1
import pentobi 1.0
import Qt.labs.settings 1.0

Window {
    id: root

    property url startUrl
    property real defaultWidth: Math.min(font.pixelSize * 48, Screen.desktopAvailableWidth)
    property real defaultHeight: Math.min(font.pixelSize * 57, Screen.desktopAvailableHeight)

    // Instead of initializing webView.url, we provide an init function that
    // needs to be called after show() to work around an issue with the initial
    // zoom factor of WebView sometimes very large on Android. Note that this
    // workaround only reduces the likelihood for this bug to occur
    // (QTBUG-58290, last occurred with Qt 5.11.2)
    function init() { webView.url = startUrl }

    width: defaultWidth; height: defaultHeight
    minimumWidth: 240; minimumHeight: 240
    x: (Screen.width - defaultWidth) / 2
    y: (Screen.height - defaultHeight) / 2
    title: qsTr("Pentobi Help")

    // Note that Android doesn't actually support multiple windows, but using
    // WebView in a window works around a bug related to QTBUG-62409, which
    // makes WebView consume Back button events, so we cannot close the help
    // window with the back key. But we need to detroy the window after closing
    // otherwise it doesn't show when made visible again.
    onClosing: if (isAndroid) helpWindow.source = ""

    WebView {
        id: webView

        anchors.fill: parent
    }
    Action {
        shortcut: "Ctrl+W"
        onTriggered: close()
    }
    Action {
        shortcut: "Alt+Left"
        onTriggered: webView.goBack()
    }
    Action {
        shortcut: "Alt+Right"
        onTriggered: webView.goForward()
    }
    Settings {
        property alias x: root.x
        property alias y: root.y
        property alias width: root.width
        property alias height: root.height

        category: "HelpWindow"
    }
}
