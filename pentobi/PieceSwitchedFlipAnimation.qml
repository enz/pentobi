//-----------------------------------------------------------------------------
/** @file pentobi/PieceSwitchedFlipAnimation.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

// Helper animation for pieces.
// Unique piece states are defined by rotating and flipping around the x axis
// but for some transitions, the shortest visual animation is flipping around
// the y axis.
SequentialAnimation {
    PropertyAction { property: "rotation"; value: rotation }
    PropertyAction {
        target: flipX; property: "angle"; value: flipX.angle
    }
    PieceRotationAnimation { target: flipY; property: "angle"; to: 180 }
    PropertyAction { target: flipY; property: "angle"; value: 0 }
}
