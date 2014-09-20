//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceManipulator.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property var pieceModel

    property alias drag: dragMouseArea.drag

    // True if piece manipulator is at a board location that is a legal move
    property bool legal: false

    signal piecePlayed

    signal positionChanged

    height: width

    Image {
        source: "images/piece-manipulator.svg"
        sourceSize { width: root.width; height: root.height }
        anchors.centerIn: root
        opacity: legal ? 0 : 0.4
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }

    Image {
        source: "images/piece-manipulator-legal.svg"
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
            onClicked: pieceModel.rotateRight()
        }
        MouseArea {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            width: 0.2 * root.width; height: 0.2 * root.height
            onClicked: pieceModel.flipAcrossX()
        }
        MouseArea {
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
            }
            width: 0.2 * root.width; height: 0.2 * root.height
            onClicked: pieceModel.flipAcrossY()
        }
        MouseArea {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            width: 0.2 * root.width; height: 0.2 * root.height
            onClicked: pieceModel.rotateLeft()
        }
    }
}
