import QtQuick 2.0

MouseArea {
    id: root

    property var pieceModel
    // True if piece manipulator is at a board location that is a legal move
    property bool legal

    signal piecePlayed

    drag { target: root; filterChildren: true }

    Image {
        anchors.fill: root
        source: theme.getImage("piece-manipulator")
        sourceSize { width: width; height: height }
        opacity: ! legal ? 0.4 : 0
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }
    Image {
        anchors.fill: root
        source: theme.getImage("piece-manipulator-legal")
        sourceSize { width: width; height: height }
        opacity: legal ? 0.4 : 0
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }
    MouseArea {
        anchors.centerIn: root
        width: 0.5 * root.width; height: width
        onClicked: piecePlayed()
    }
    MouseArea {
        anchors { top: root.top; horizontalCenter: root.horizontalCenter }
        width: 0.2 * root.width; height: width
        onClicked: pieceModel.rotateRight()
    }
    MouseArea {
        anchors { right: root.right; verticalCenter: root.verticalCenter }
        width: 0.2 * root.width; height: width
        onClicked: pieceModel.flipAcrossX()
    }
    MouseArea {
        anchors { bottom: root.bottom; horizontalCenter: root.horizontalCenter }
        width: 0.2 * root.width; height: width
        onClicked: pieceModel.flipAcrossY()
    }
    MouseArea {
        anchors { left: root.left; verticalCenter: root.verticalCenter }
        width: 0.2 * root.width; height: width
        onClicked: pieceModel.rotateLeft()
    }
}
