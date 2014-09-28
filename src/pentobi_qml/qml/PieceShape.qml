//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceShape.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Transformable {
    id: root

    property string colorName
    property bool isTrigon
    property var elements // List of points with coordinates of piece elements
    property point center
    property real gridElementWidth
    property real gridElementHeight
    property bool fastRendering

    Item {
        id: pieceElements

        width: 10 * gridElementWidth
        height: 10 * gridElementHeight
        x: -width / 2
        y: -height / 2
        z: 1

        Repeater {
            model: elements
            PieceElement {
                colorName: root.colorName
                isTrigon: root.isTrigon
                isDownward: isTrigon && ((modelData.x % 2 != 0) ? (modelData.y % 2 == 0) : (modelData.y % 2 != 0))
                gridElementWidth: root.gridElementWidth
                gridElementHeight: root.gridElementHeight
                fastRendering: root.fastRendering
                x: (_isTrigon ?
                        (modelData.x - center.x - 0.5) * gridElementWidth :
                        (modelData.x - center.x) * gridElementWidth) +
                   pieceElements.width / 2
                y: (modelData.y - center.y) * gridElementHeight +
                   pieceElements.height / 2
                angle: root.rotation
                flipX: Math.abs(flipXAngle % 360 - 180) < 90
                flipY: Math.abs(flipYAngle % 360 - 180) < 90
            }
        }
    }
}
