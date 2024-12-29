//-----------------------------------------------------------------------------
/** @file pentobi/ScoreElement2.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Item {
    id: root

    property alias color1: point1.color
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
        implicitWidth: 0.7 * root.fontSize
        implicitHeight: 0.7 * root.fontSize
        opacity: root.isAltColor && root.isFinal ? 0 : 1
        radius: width / 2
    }
    Rectangle {
        id: point2

        anchors {
            verticalCenter: parent.verticalCenter
            left: point1.right
        }
        implicitWidth: 0.7 * root.fontSize
        implicitHeight: 0.7 * root.fontSize
        color: root.isAltColor && root.isFinal ? root.color1 : root.color2
        radius: width / 2
    }
    Text {
        id: text

        anchors {
            verticalCenter: parent.verticalCenter
            left: point2.right
            leftMargin: 0.14 * font.pixelSize
        }
        text: root.isAltColor ? ""
                              : root.isFinal ?
                                    "<u>%L1</u>".arg(root.value)
                                  : "%L1".arg(root.value)
        color: theme.colorText
        opacity: 0.8
        font.preferShaping: false
    }
}
