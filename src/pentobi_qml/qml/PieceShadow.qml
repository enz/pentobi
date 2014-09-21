//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceShadow.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0

Transformable {
    id: root

    property bool isTrigon
    property var elements // ListElement of points with coordinates of piece elements
    property point center
    property real gridElementWidth
    property real gridElementHeight
    property real imageSourceWidth
    property real imageSourceHeight

    Repeater {
        model: elements
        Image {
            property bool isDownward: (modelData.x % 2 != 0) ? (modelData.y % 2 == 0) : (modelData.y % 2 != 0)
            width: isTrigon ? 2 * gridElementWidth : gridElementWidth
            height: gridElementHeight
            source: {
                if (isTrigon) {
                    if (isDownward) return "images/triangle-down-shadow.svg"
                    else return "images/triangle-shadow.svg"
                }
                else return "images/square-shadow.svg"
            }
            sourceSize { width: imageSourceWidth; height: imageSourceHeight }
            x: isTrigon ? (modelData.x - center.x - 0.5) * gridElementWidth : (modelData.x - center.x) * gridElementWidth
            y: (modelData.y - center.y) * gridElementHeight
        }
    }
}
