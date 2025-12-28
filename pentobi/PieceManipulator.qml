//-----------------------------------------------------------------------------
/** @file pentobi/PieceManipulator.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

Item {
    id: root

    property PieceModel pieceModel
    // True if piece manipulator is at a board location that is a legal move
    property bool legal

    // Fast move animation to make sure that the next grid cell is reached
    // before the next auto-repeat keyboard command
    property bool fastMove: false

    property real buttonSize: 0.15 * root.width

    property real animationDuration:
        ! pieceModel || ! gameView.enableAnimations ?
            0 : fastMove ? 50 : animationDurationMove

    signal piecePlayed

    enabled: pieceModel

    Image {
        anchors.fill: root
        source: theme.getImage("piece-manipulator")
        sourceSize { width: 800; height: 800 }
        opacity: root.pieceModel && ! root.legal ? 0.7 : 0

        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
    }
    Image {
        anchors.fill: root
        source: theme.getImage("piece-manipulator-legal")
        sourceSize { width: 800; height: 800 }
        opacity: root.pieceModel && root.legal ? 0.55 : 0

        Behavior on opacity { NumberAnimation { duration: animationDurationFast } }
    }
    MouseArea {
        id: dragArea

        anchors.centerIn: root
        // Make drag area a bit larger than image to avoid accidental
        // flicking in PieceSelectorMobile when wanting to drag
        width: root.width + root.buttonSize; height: width
        drag {
            target: root
            filterChildren: true
            minimumX: -root.width / 2
            maximumX: root.parent.width - root.width / 2
            minimumY: -root.height / 2
            maximumY: root.parent.height - root.height / 2
        }
        // Consume mouse hover events in case it is over toolbar
        hoverEnabled: true

        MouseArea {
            anchors.centerIn: dragArea
            width: 0.9 * (root.width - 2 * root.buttonSize); height: width
            onClicked: root.piecePlayed()
        }
        MouseArea {
            anchors {
                top: dragArea.top
                margins: (dragArea.width - root.width) / 2
                horizontalCenter: dragArea.horizontalCenter
            }
            width: root.buttonSize; height: width
            onClicked: root.pieceModel.rotateRight()
        }
        MouseArea {
            anchors {
                right: dragArea.right
                margins: (dragArea.width - root.width) / 2
                verticalCenter: dragArea.verticalCenter
            }
            width: root.buttonSize; height: width
            onClicked: root.pieceModel.flipAcrossX()
        }
        MouseArea {
            anchors {
                bottom: dragArea.bottom
                margins: (dragArea.width - root.width) / 2
                horizontalCenter: dragArea.horizontalCenter
            }
            width: root.buttonSize; height: width
            onClicked: root.pieceModel.flipAcrossY()
        }
        MouseArea {
            anchors {
                left: dragArea.left
                margins: (dragArea.width - root.width) / 2
                verticalCenter: dragArea.verticalCenter
            }
            width: root.buttonSize; height: width
            onClicked: root.pieceModel.rotateLeft()
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
