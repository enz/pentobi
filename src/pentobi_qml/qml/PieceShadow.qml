//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceShadow.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import pentobi 1.0

Transformable {
    id: root

    property string gameVariant
    property var elements // ListElement of points with coordinates of piece elements
    property point center
    property real gridElementWidth
    property real gridElementHeight
    property real imageSourceWidth
    property real imageSourceHeight

    property bool _isTrigon: gameVariant.indexOf("trigon") >= 0

    Repeater {
        model: elements
        Image {
            property bool isDownward: (modelData.x % 2 != 0) ? (modelData.y % 2 == 0) : (modelData.y % 2 != 0)
            width: _isTrigon ? 2 * gridElementWidth : gridElementWidth
            height: gridElementHeight
            source: _isTrigon ? "images/triangle-shadow.svg" : "images/square-shadow.svg"
            sourceSize { width: imageSourceWidth; height: imageSourceHeight }
            x: _isTrigon ? (modelData.x - center.x - 0.5) * gridElementWidth : (modelData.x - center.x) * gridElementWidth
            y: (modelData.y - center.y) * gridElementHeight
            transform: Rotation {
                axis { x: 1; y: 0; z: 0 }
                origin { x: width / 2; y: height / 2 }
                angle: (_isTrigon && isDownward) ? 180 : 0
            }
        }
    }
}
