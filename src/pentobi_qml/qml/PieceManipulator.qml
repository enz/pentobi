import QtQuick 2.0
import QtQuick.Window 2.0

Item {
    id: root

    property var pieceModel
    // True if piece manipulator is at a board location that is a legal move
    property bool legal
    property real innerSize
    property real buttonSize: Math.min(innerSize * 0.4, 10 * Screen.pixelDensity)

    signal piecePlayed

    width: innerSize + 2 * buttonSize; height: width

    Rectangle {
        anchors.fill: root
        color: "#70716d"
        radius: width / 2
        opacity: 0.3
    }
    Rectangle {
        anchors.centerIn: root
        color: "white"
        width: innerSize; height: width
        radius: width / 2
        opacity: legal ? 0.3 : 0.1
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }
    Image {
        source: theme.getImage("rotate")
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        width: buttonSize; height: width
        sourceSize { width: width; height: height }
        opacity: 0.3
    }
    Image {
        source: theme.getImage("rotate")
        mirror: true
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        width: buttonSize; height: width
        sourceSize { width: width; height: height }
        opacity: 0.3
    }
    Image {
        source: theme.getImage("flip")
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        width: buttonSize; height: width
        sourceSize { width: width; height: height }
        opacity: 0.3
    }
    Image {
        source: theme.getImage("flip")
        rotation: 90
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        width: buttonSize; height: width
        sourceSize { width: width; height: height }
        opacity: 0.3
    }
    MouseArea {
        id: dragArea

        anchors.fill: root
        drag {
            target: root
            filterChildren: true
            minimumX: -width / 2; maximumX: root.parent.width - width / 2
            minimumY: -height / 2; maximumY: root.parent.height - height / 2
        }

        MouseArea {
            anchors.centerIn: dragArea
            width: innerSize; height: width
            onClicked: piecePlayed()
        }
        MouseArea {
            anchors {
                top: dragArea.top
                horizontalCenter: dragArea.horizontalCenter
            }
            width: buttonSize; height: width
            onClicked: pieceModel.rotateRight()
        }
        MouseArea {
            anchors {
                right: dragArea.right
                verticalCenter: dragArea.verticalCenter
            }
            width: buttonSize; height: width
            onClicked: pieceModel.flipAcrossX()
        }
        MouseArea {
            anchors {
                bottom: dragArea.bottom
                horizontalCenter: dragArea.horizontalCenter
            }
            width: buttonSize; height: width
            onClicked: pieceModel.flipAcrossY()
        }
        MouseArea {
            anchors { left: dragArea.left; verticalCenter: dragArea.verticalCenter }
            width: buttonSize; height: width
            onClicked: pieceModel.rotateLeft()
        }
    }
}
