//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/ScoreElement2.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property alias color1: point1.color
    property alias color2: point2.color
    property bool isFinal: false
    property int value: 0

    Rectangle {
        id: point1

        width: root.height / 2
        height: width
        radius: width / 2
        anchors.verticalCenter: root.verticalCenter
    }
    Rectangle {
        id: point2

        width: point1.width
        height: point1.height
        radius: point1.radius
        anchors.left: point1.right
        anchors.verticalCenter: root.verticalCenter
    }
    Text {
        text: value
        color: "#50494C"
        width: root.width - point1.width - point2.width
        height: root.height
        anchors.left: point2.right
        anchors.leftMargin: 0.3 * point1.width
        verticalAlignment: Text.AlignVCenter
        renderType: Text.NativeRendering
        font.underline: isFinal
        font.pixelSize: height
    }
}
