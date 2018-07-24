//-----------------------------------------------------------------------------
/** @file pentobi/qml/HelpWindow.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import QtQuick.Controls 2.3
import QtWebView 1.1
import pentobi 1.0
import Qt.labs.settings 1.0

Window {
    id: root

    property url startUrl: {
        var lang = Qt.locale().name
        var pos = lang.indexOf("_")
        if (pos >= 0)
            lang = lang.substr(0, pos)
        if (lang !== "C" && lang !== "de")
            lang = "C"
        // qrc urls work in WebView on Linux but not on Android
        if (isDesktop)
            return "qrc:///qml/help/" + lang + "/pentobi/index.html"
        return helpFileExtractor.extract(lang)
    }
    property real defaultWidth: Math.min(font.pixelSize * 40, Screen.desktopAvailableWidth)
    property real defaultHeight: Math.min(font.pixelSize * 45, Screen.desktopAvailableHeight)

    // Instead of initializing webView.url with startUrl, we provide an init
    // function that needs to be called after show() to work around an issue
    // with the initial zoom factor of WebView sometimes very large on Android
    // (last tested with Qt 5.9.2)
    function init() { if (isAndroid) webView.url = startUrl }

    width: defaultWidth; height: defaultHeight
    minimumWidth: 128; minimumHeight: 128
    x: (Screen.width - defaultWidth) / 2
    y: (Screen.height - defaultHeight) / 2
    title: qsTr("Pentobi Help")

    // We'd like to hide the window instead of closing it but it doesn't work on
    // Android to make it visible again (problem with WebView? Last tested with
    // Qt 5.8-rc), so for now we destroy it and recreate it when needed.
    onClosing: if (isAndroid) helpWindow.sourceComponent = null

    WebView {
        id: webView

        // See comment at init()
        url: isAndroid ? undefined : startUrl
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
