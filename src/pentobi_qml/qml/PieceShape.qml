//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceShape.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Transformable {
    id: root

    property string gameVariant
    property int color
    property var elements // List of points with coordinates of piece elements
    property point center
    property real gridElementWidth
    property real gridElementHeight
    property real imageSourceWidth
    property real imageSourceHeight

    property bool _isTrigon: gameVariant.indexOf("trigon") >= 0

    Repeater {
        model: elements
        PieceElement {
            gridElementWidth: root.gridElementWidth
            gridElementHeight: root.gridElementHeight
            imageSourceWidth: root.imageSourceWidth
            imageSourceHeight: root.imageSourceHeight
            x: _isTrigon ? (modelData.x - center.x - 0.5) * gridElementWidth : (modelData.x - center.x) * gridElementWidth
            y: (modelData.y - center.y) * gridElementHeight

            // TODO:  Only correct for Trigon boards with even size, negate for
            // odd size
            isDownward: _isTrigon && ((modelData.x % 2 != 0) ? (modelData.y % 2 == 0) : (modelData.y % 2 != 0))

            gameVariant: root.gameVariant
            color: root.color
            angle: root.rotation
            flipX: Math.abs(flipXAngle % 360 - 180) < 90
            flipY: Math.abs(flipYAngle % 360 - 180) < 90
        }
    }
}
