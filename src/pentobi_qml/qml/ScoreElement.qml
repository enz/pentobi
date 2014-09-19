//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/ScoreElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property alias color: point.color
    property bool isFinal: false
    property int value
    property real pointSize

    signal clicked

    width: 5 * pointSize

    MouseArea {
        anchors.fill: root
        onClicked: root.clicked()
    }
    Rectangle {
        id: point

        width: pointSize
        height: width
        radius: width / 2
        anchors.verticalCenter: root.verticalCenter
    }
    Text {
        id: scoreText

        text: value
        color: "#50494C"
        anchors {
            left: point.right
            leftMargin: 0.3 * point.width
            verticalCenter: root.verticalCenter
        }
        verticalAlignment: Text.AlignVCenter
        renderType: Text.NativeRendering
        font.underline: isFinal
        font.pixelSize: 1.7 * point.width
    }
}
