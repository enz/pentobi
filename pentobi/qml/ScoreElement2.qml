//-----------------------------------------------------------------------------
/** @file pentobi/qml/ScoreElement2.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Item {
    id: root

    property color color1
    property color color2
    property bool isFinal
    property bool isAltColor
    property real value
    property alias fontSize: text.font.pixelSize

    implicitWidth: point1.implicitWidth + point1.implicitWidth
                   + text.implicitWidth + text.anchors.leftMargin
    implicitHeight: Math.max(point1.implicitHeight, point2.implicitHeight,
                             text.implicitHeight)

    Rectangle {
        id: point1

        anchors.verticalCenter: parent.verticalCenter
        implicitWidth: 0.7 * fontSize
        implicitHeight: 0.7 * fontSize
        color: color1
        opacity: isAltColor && isFinal ? 0 : 1
        radius: width / 2
    }
    Rectangle {
        id: point2

        anchors {
            verticalCenter: parent.verticalCenter
            left: point1.right
        }
        implicitWidth: 0.7 * fontSize
        implicitHeight: 0.7 * fontSize
        color: isAltColor && isFinal ? color1 : color2
        radius: width / 2
    }
    Text {
        id: text

        anchors {
            verticalCenter: parent.verticalCenter
            left: point2.right
            leftMargin: 0.14 * font.pixelSize
        }
        text: isAltColor ? ""
                         : isFinal ? "<u>%L1</u>".arg(value) : "%L1".arg(value)
        color: theme.colorText
        opacity: 0.8
        font.preferShaping: false
    }
}
