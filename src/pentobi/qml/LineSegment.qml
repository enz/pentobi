//-----------------------------------------------------------------------------
/** @file pentobi/qml/LineSegment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// Piece element for Nexos. See Square.qml for comments.
Item {
    id: root

    property bool isHorizontal

    Loader {
        anchors.fill: root
        opacity: imageOpacity0
        sourceComponent: opacity > 0 || item ? component0 : null

        Component {
            id: component0

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
                mirror: ! isHorizontal
                rotation: isHorizontal ? 0 : -90
            }
        }
    }
    Loader {
        anchors.fill: root
        opacity: imageOpacity90
        sourceComponent: opacity > 0 || item ? component90 : null

        Component {
            id: component90

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
                mirror: isHorizontal
                rotation: isHorizontal ? -180 : -90
            }
        }
    }
    Loader {
        anchors.fill: root
        opacity: imageOpacity180
        sourceComponent: opacity > 0 || item ? component180 : null

        Component {
            id: component180

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
                mirror: ! isHorizontal
                rotation: isHorizontal ? -180 : -270
            }
        }
    }
    Loader {
        anchors.fill: root
        opacity: imageOpacity270
        sourceComponent: opacity > 0 || item ? component270 : null

        Component {
            id: component270

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
                mirror: isHorizontal
                rotation: isHorizontal ? 0 : -270
            }
        }
    }
}
