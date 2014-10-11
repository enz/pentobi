import QtQuick 2.0

Rectangle {
    id: root

    property real fontSize

    property real _margin: 0.5 * fontSize

    signal clicked()

    function clear() {
        opacity = 0
    }
    function show(text) {
        messageText.text = text
        opacity = 0.8
    }

    width: messageText.contentWidth + 2 * _margin
    height: messageText.contentHeight + 2 * _margin
    radius: _margin
    color: theme.backgroundColorMessage
    opacity: 0

    Text {
        id: messageText
        font.pixelSize: root.fontSize
        color: theme.fontColorMessage
        renderType: Text.NativeRendering
        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: root
        enabled: root.opacity > 0
        onClicked: root.clicked()
    }
    Behavior on opacity { NumberAnimation { duration: 400 } }
}
