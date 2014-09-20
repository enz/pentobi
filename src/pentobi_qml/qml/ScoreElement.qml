//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/ScoreElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property alias color: point.color
    property bool isFinal
    property bool isToPlay
    property int value
    property real pointSize

    width: 5 * pointSize

    Rectangle {
        id: point

        z: 1
        width: (isToPlay ? 1.3 : 1) * pointSize
        border {
            color: Qt.darker(color)
            width: isToPlay ? Math.max(0.15 * pointSize, 1) : 0
        }
        height: width
        radius: width / 2
        anchors.verticalCenter: root.verticalCenter
    }
    Text {
        id: scoreText

        z: 1
        text: value
        color: "#5A5255"
        anchors {
            left: point.right
            leftMargin: (isToPlay ? 0.2 : 0.5) * point.width
            verticalCenter: root.verticalCenter
        }
        verticalAlignment: Text.AlignVCenter
        renderType: Text.NativeRendering
        font.underline: isFinal
        font.pixelSize: 1.7 * pointSize
    }
}
