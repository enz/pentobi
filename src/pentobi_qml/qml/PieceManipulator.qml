import QtQuick 2.0

Item {
    id: root

    property var pieceModel

    property alias drag: dragMouseArea.drag

    // True if piece manipulator is at a board location that is a legal move
    property bool legal: false

    signal piecePlayed
    signal orientationChanged

    height: width

    Image {
        source: theme.getImage("piece-manipulator")
        sourceSize { width: root.width; height: root.height }
        anchors.centerIn: root
        opacity: legal ? 0 : 0.4
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }
    Image {
        source: theme.getImage("piece-manipulator-legal")
        sourceSize { width: root.width; height: root.height }
        anchors.centerIn: root
        opacity: legal ? 0.4 : 0
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }
    MouseArea {
        id: dragMouseArea

        anchors { centerIn: root; fill: root }
        drag { target: root; filterChildren: true }

        MouseArea {
            // Mouse area slightly larger than the image that consumes clicks
            // close to the piece manipulator to avoid that the user
            // accidentally selects a piece when he wants to click on one of
            // the rotation/flip mouse areas and the piece manipulator is above
            // the piece selector.
            anchors.centerIn: parent
            width: 1.1 * root.width; height: 1.1 * root.height
        }
        MouseArea {
            anchors.centerIn: parent
            width: 0.5 * root.width; height: 0.5 * root.height
            onClicked: piecePlayed()
        }
        MouseArea {
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            width: 0.2 * root.width; height: 0.2 * root.height
            onClicked: { pieceModel.rotateRight(); orientationChanged() }
        }
        MouseArea {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            width: 0.2 * root.width; height: 0.2 * root.height
            onClicked: { pieceModel.flipAcrossX(); orientationChanged() }
        }
        MouseArea {
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
            width: 0.2 * root.width; height: 0.2 * root.height
            onClicked: { pieceModel.flipAcrossY(); orientationChanged() }
        }
        MouseArea {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            width: 0.2 * root.width; height: 0.2 * root.height
            onClicked: { pieceModel.rotateLeft(); orientationChanged() }
        }
    }
}
