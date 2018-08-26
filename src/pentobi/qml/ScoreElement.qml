//-----------------------------------------------------------------------------
/** @file pentobi/qml/ScoreElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property alias color: point.color
    property bool isFinal
    property real value
    property real bonus
    property real pointSize

    Rectangle {
        id: point

        width: pointSize
        height: width
        radius: width / 2
        anchors.verticalCenter: root.verticalCenter
    }
    Text {
        id: scoreText

        text: ! isFinal ?
                  "%L1".arg(value) :
                  "%1<u>%L2</u>".arg(bonus > 0 ? "*" : "").arg(value)
        color: theme.colorText
        opacity: 0.9 - 0.15 * theme.colorBackground.hslLightness
        anchors {
            left: point.right
            leftMargin: 0.2 * point.width
            verticalCenter: root.verticalCenter
        }
        verticalAlignment: Text.AlignVCenter
        font { pixelSize: 1.4 * pointSize; preferShaping: false }
    }
}
