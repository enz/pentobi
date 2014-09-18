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

    Rectangle {
        id: point

        width: root.height / 2
        height: width
        radius: width / 2
        anchors.verticalCenter: root.verticalCenter
    }
    Text {
        id: scoreText

        text: value
        color: "#50494C"
        width: root.width - point.width
        height: root.height
        anchors.left: point.right
        anchors.leftMargin: 0.3 * point.width
        verticalAlignment: Text.AlignVCenter
        renderType: Text.NativeRendering
        font.underline: isFinal
        font.pixelSize: height
    }
}
