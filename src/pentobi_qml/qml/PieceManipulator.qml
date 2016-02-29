import QtQuick 2.0

Item {
    id: root

    property var pieceModel
    // True if piece manipulator is at a board location that is a legal move
    property bool legal
    property int minimumX
    property int maximumX
    property int minimumY
    property int maximumY

    signal piecePlayed

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
        id: dragArea

        anchors.fill: root
        drag {
            target: root
            filterChildren: true
            minimumX: root.minimumX; maximumX: root.maximumX
            minimumY: root.minimumY; maximumY: root.maximumY
        }

        MouseArea {
            anchors.centerIn: dragArea
            width: 0.5 * root.width; height: width
            onClicked: piecePlayed()
        }
        MouseArea {
            anchors {
                top: dragArea.top
                horizontalCenter: dragArea.horizontalCenter
            }
            width: 0.2 * root.width; height: width
            onClicked: pieceModel.rotateRight()
        }
        MouseArea {
            anchors {
                right: dragArea.right
                verticalCenter: dragArea.verticalCenter
            }
            width: 0.2 * root.width; height: width
            onClicked: pieceModel.flipAcrossX()
        }
        MouseArea {
            anchors {
                bottom: dragArea.bottom
                horizontalCenter: dragArea.horizontalCenter
            }
            width: 0.2 * root.width; height: width
            onClicked: pieceModel.flipAcrossY()
        }
        MouseArea {
            anchors { left: dragArea.left; verticalCenter: dragArea.verticalCenter }
            width: 0.2 * root.width; height: width
            onClicked: pieceModel.rotateLeft()
        }
    }
}
