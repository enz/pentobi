//-----------------------------------------------------------------------------
/** @file pentobi/qml/PieceManipulator.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Item {
    id: root

    property QtObject pieceModel
    // True if piece manipulator is at a board location that is a legal move
    property bool legal

    // Fast move animation to make sure that the next grid cell is reached
    // before the next auto-repeat keyboard command
    property bool fastMove: false

    property bool isOutsideBoard

    // Manipulator buttons are smaller on desktop with mouse usage
    property real buttonSize: (isDesktop ? 0.12 : 0.17) * root.width

    property real animationDuration:
        ! pieceModel || ! gameDisplay.enableAnimations ?
            0 : fastMove ? 50 : animationDurationMove

    signal piecePlayed

    enabled: pieceModel

    Image {
        anchors.fill: root
        source: isDesktop ? theme.getImage("piece-manipulator-desktop")
                          : theme.getImage("piece-manipulator")
        sourceSize { width: width; height: height }
        opacity: {
            if (! pieceModel) return 0
            if (isOutsideBoard) return 0.7
            if (legal) return 0.4
            return 0.5
        }

        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
    }
    Rectangle {
        anchors.centerIn: parent
        opacity: pieceModel && legal ? 0.45 : 0
        color: theme.colorPieceManipulatorLegal
        width: (isDesktop ? 0.74 : 0.64) * root.width; height: width
        radius: width / 2

        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
    }
    Rectangle {
        anchors.centerIn: parent
        opacity: {
            if (! pieceModel || legal) return 0
            if (isOutsideBoard) return 0.35
            return 0.45
        }
        color: isOutsideBoard ? theme.colorBackground : theme.colorPieceManipulator
        width: (isDesktop ? 0.74 : 0.64) * root.width; height: width
        radius: width / 2

        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
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

    Behavior on x {
        NumberAnimation {
            duration: animationDuration
            easing.type: Easing.InOutSine
        }
    }
    Behavior on y {
        NumberAnimation {
            duration: animationDuration
            easing.type: Easing.InOutSine
        }
    }
}
