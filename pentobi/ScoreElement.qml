//-----------------------------------------------------------------------------
/** @file pentobi/ScoreElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Item {
    property alias color: point.color
    property bool isFinal
    property real value
    property real bonus
    property alias fontSize: text.font.pixelSize

    implicitWidth: point.implicitWidth + text.implicitWidth
                   + text.anchors.leftMargin
    implicitHeight: Math.max(point.implicitHeight, text.implicitHeight)

    Rectangle {
        id: point

        anchors.verticalCenter: parent.verticalCenter
        implicitWidth: 0.7 * fontSize
        implicitHeight: 0.7 * fontSize
        radius: width / 2
    }
    Text {
        id: text

        anchors {
            verticalCenter: parent.verticalCenter
            left: point.right
            leftMargin: 0.14 * font.pixelSize
        }
        text: ! isFinal ?
                  "%L1".arg(value) :
                  "%1<u>%L2</u>".arg(bonus > 0 ? "★" : "").arg(value)
        color: theme.colorText
        opacity: 0.8
        font.preferShaping: false
    }
}
