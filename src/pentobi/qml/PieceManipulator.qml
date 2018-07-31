//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceManipulator.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property var pieceModel
    // True if piece manipulator is at a board location that is a legal move
    property bool legal

    // Fast move animation to make sure that the next grid cell is reached
    // before the next auto-repeat keyboard command
    property bool fastMove: false

    // Manipulator buttons are smaller on desktop with mouse usage
    property real buttonSize: (isDesktop ? 0.12 : 0.17) * root.width

    property real animationsDuration:
        ! visible || ! gameDisplay.enableAnimations ? 0 : fastMove ? 50 : 300

    signal piecePlayed

    Rectangle {
        anchors.fill: parent
        opacity: ! legal ? 0.34 : 0
        radius: width / 2
        color: theme.colorPieceManipulatorBase
        border {
            width: buttonSize
            color: theme.colorPieceManipulatorBorder
        }
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }
    Rectangle {
        anchors.fill: parent
        opacity: legal ? 0.34 : 0
        radius: width / 2
        color: theme.colorPieceManipulatorLegal
        border {
            width: buttonSize
            color: theme.colorPieceManipulatorBorder
        }
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }
    Image {
        anchors {
            top: root.top
            horizontalCenter: root.horizontalCenter
        }
        source: theme.getImage("piece-manipulator-rotate")
        sourceSize { width: buttonSize; height: buttonSize }
        opacity: 0.5
    }
    Image {
        anchors {
            left: root.left
            verticalCenter: root.verticalCenter
        }
        source: theme.getImage("piece-manipulator-rotate")
        mirror: true
        sourceSize { width: buttonSize; height: buttonSize }
        opacity: 0.5
    }
    Image {
        anchors {
            right: root.right
            verticalCenter: root.verticalCenter
        }
        source: theme.getImage("piece-manipulator-flip")
        sourceSize { width: buttonSize; height: buttonSize }
        opacity: 0.5
    }
    Image {
        anchors {
            bottom: root.bottom
            horizontalCenter: root.horizontalCenter
        }
        source: theme.getImage("piece-manipulator-flip")
        rotation: 90
        sourceSize { width: buttonSize; height: buttonSize }
        opacity: 0.5
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
        // Consume mouse hover events in case it is over PieceList
        hoverEnabled: isDesktop

        MouseArea {
            anchors.centerIn: dragArea
            width: 0.9 * (root.width - 2 * buttonSize); height: width
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

    Behavior on x { NumberAnimation { duration: animationsDuration; easing.type: Easing.InOutSine } }
    Behavior on y { NumberAnimation { duration: animationsDuration; easing.type: Easing.InOutSine } }
}
