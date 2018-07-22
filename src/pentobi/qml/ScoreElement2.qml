//-----------------------------------------------------------------------------
/** @file pentobi/qml/ScoreElement2.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property color color1
    property color color2
    property bool isFinal
    property bool isToPlay
    property bool isAltColor
    property real value
    property real pointSize

    Rectangle {
        id: point1

        color: color1
        opacity: isAltColor && isFinal ? 0 : 1
        width: (isToPlay ? 1.3 : 1) * pointSize
        border {
            color: Qt.lighter(color1, theme.lighterToPlay)
            width: isToPlay ? Math.max(0.15 * pointSize, 1) : 0
        }
        height: width
        radius: width / 2
        anchors.verticalCenter: root.verticalCenter
    }
    Rectangle {
        id: point2

        color: isAltColor && isFinal ? color1 : color2
        width: pointSize
        height: width
        radius: width / 2
        anchors {
            left: point1.right
            verticalCenter: root.verticalCenter
        }
    }
    Text {
        text: {
            if (isAltColor)
                return isFinal ? "(<u>%L1</u>)".arg(value) : "(%L1)".arg(value)
            else
                return isFinal ? "<u>%L1</u>".arg(value) : "%L1".arg(value)
        }
        color: theme.colorText
        opacity: theme.opacityScore
        width: root.width - point1.width - point2.width - anchors.leftMargin
        anchors {
            left: point2.right
            leftMargin: (isToPlay ? 0.2 : 0.4) * point1.width
            verticalCenter: root.verticalCenter
        }
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 1.4 * pointSize
    }
}
