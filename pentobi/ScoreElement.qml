//-----------------------------------------------------------------------------
/** @file pentobi/ScoreElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Item {
    id: root

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
        implicitWidth: 0.7 * root.fontSize
        implicitHeight: 0.7 * root.fontSize
        radius: width / 2
    }
    Text {
        id: text

        anchors {
            verticalCenter: parent.verticalCenter
            left: point.right
            leftMargin: 0.14 * font.pixelSize
        }
        text: ! root.isFinal ?
                  "%L1".arg(root.value) :
                  "<u>%L1</u>%2".arg(root.value).arg(root.bonus > 0 ? "<sup>*</sup>" : "")
        color: theme.colorText
        opacity: 0.8
    }
}
