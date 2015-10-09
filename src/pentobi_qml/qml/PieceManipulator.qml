import QtQuick 2.0

MouseArea {
    id: root

    property var pieceModel
    // True if piece manipulator is at a board location that is a legal move
    property bool legal

    signal piecePlayed

    drag { target: root; filterChildren: true }
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
        // Mouse area slightly larger than the image that consumes clicks close
        // to the piece manipulator to avoid that the user accidentally selects
        // a piece when he wants to click on one of the rotation/flip mouse
        // areas and the piece manipulator is above the piece selector.
        anchors.centerIn: root
        width: 1.1 * root.width; height: 1.1 * root.height
    }
    MouseArea {
        anchors.centerIn: root
        width: 0.5 * root.width; height: 0.5 * root.height
        onClicked: piecePlayed()
    }
    MouseArea {
        anchors { top: root.top; horizontalCenter: root.horizontalCenter }
        width: 0.2 * root.width; height: 0.2 * root.height
        onClicked: pieceModel.rotateRight()
    }
    MouseArea {
        anchors { right: root.right; verticalCenter: root.verticalCenter }
        width: 0.2 * root.width; height: 0.2 * root.height
        onClicked: pieceModel.flipAcrossX()
    }
    MouseArea {
        anchors { bottom: root.bottom; horizontalCenter: root.horizontalCenter }
        width: 0.2 * root.width; height: 0.2 * root.height
        onClicked: pieceModel.flipAcrossY()
    }
    MouseArea {
        anchors { left: root.left; verticalCenter: root.verticalCenter }
        width: 0.2 * root.width; height: 0.2 * root.height
        onClicked: pieceModel.rotateLeft()
    }
}
