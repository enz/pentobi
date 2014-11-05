import QtQuick 2.0

Item {
    id: root

    property alias color1: point1.color
    property alias color2: point2.color
    property bool isFinal
    property bool isToPlay
    property bool isAltColor
    property int value
    property real pointSize

    Rectangle {
        id: point1

        width: (isToPlay ? 1.3 : 1) * pointSize
        border {
            color: Qt.lighter(color1, theme.toPlayColorLighter)
            width: isToPlay ? Math.max(0.15 * pointSize, 1) : 0
        }
        height: width
        radius: width / 2
        anchors.verticalCenter: root.verticalCenter
    }
    Rectangle {
        id: point2

        width: pointSize
        height: width
        radius: width / 2
        anchors.left: point1.right
        anchors.verticalCenter: root.verticalCenter
    }
    Text {
        text: isAltColor ? "(" + value + ")" : value
        color: theme.fontColorScore
        width: root.width - point1.width - point2.width - anchors.leftMargin
        anchors {
            left: point2.right
            leftMargin: (isToPlay ? 0.1 : 0.4) * point1.width
            verticalCenter: root.verticalCenter
        }
        verticalAlignment: Text.AlignVCenter
        renderType: Text.NativeRendering
        font.underline: isFinal
        font.pixelSize: 1.9 * pointSize
    }
}
