import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import QtWebView 1.1
import pentobi 1.0

Window {
    property string language: {
        var s = Qt.locale().name
        var pos = s.indexOf("_")
        if (pos >= 0)
            s = s.substr(0, pos)
        if (s !== "C" && s !== "de")
            s = "C"
        return s
    }
    property url startUrl:
        if (isAndroid)
            return helpFileExtractor.extract(language)
        else
            return "qrc:///qml/help/" + language + "/pentobi/index.html"

    width: isAndroid ? Screen.desktopAvailableWidth : Math.min(Screen.pixelDensity * 150, Screen.desktopAvailableWidth)
    height: isAndroid ? Screen.desktopAvailableHeight : Math.min(Screen.pixelDensity * 180, Screen.desktopAvailableHeight)
    title: qsTr("Pentobi Help")
    color: theme.backgroundColor
    flags: Qt.Dialog
    // We'd like to hide the window instead of closing it but it doesn't work on
    // Android to make it visible again (propblem with WebView? Last tested with
    // Qt 5.8-rc), so for now we destroy it and recreate it when needed.
    onClosing: helpWindowLoader.source = ""

    WebView {
        id: webView

        anchors.fill: parent
        url: startUrl
    }
    HelpFileExtractor { id: helpFileExtractor }
}
