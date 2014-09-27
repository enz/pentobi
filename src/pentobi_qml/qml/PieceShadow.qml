//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/PieceShadow.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtGraphicalEffects 1.0

Transformable {
    id: root

    /** List of points with coordinates of piece elements */
    property var elements

    /** Center of the piece elements */
    property point center

    property bool isTrigon
    property real gridElementWidth
    property real gridElementHeight

    Item {
        id: shadowImages

        width: 10 * gridElementWidth
        height: 10 * gridElementHeight
        x: -width / 2
        y: -height / 2
        visible: false

        Repeater {
            model: elements
            Image {
                property bool isDownward: (modelData.x % 2 != 0) ?
                                              (modelData.y % 2 == 0) :
                                              (modelData.y % 2 != 0)

                width: isTrigon ? 2 * gridElementWidth : gridElementWidth
                height: gridElementHeight
                source: theme.getImage(isTrigon ? "triangle-shadow" :
                                                  "square-shadow")
                sourceSize {
                    width: isTrigon ? 2 * imageSourceWidth :
                                      imageSourceWidth
                    height: imageSourceHeight
                }
                smooth: false
                x: (isTrigon ?
                        (modelData.x - center.x - 0.5) * gridElementWidth :
                        (modelData.x - center.x) * gridElementWidth) +
                   shadowImages.width / 2
                y: (modelData.y - center.y) * gridElementHeight +
                   shadowImages.height / 2
                rotation: isTrigon && isDownward ? 180 : 0
            }
        }
    }
    FastBlur {
        anchors.fill: shadowImages
        source: shadowImages
        radius: 32
        cached: true
    }
}
