//-----------------------------------------------------------------------------
/** @file pentobi/qml/QuarterSquare.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// Piece element used in GembloQ. See Square.qml for comments
Item {
    id: root

    property int pointType

    Loader {
        id: loaderTop

        function loadImage() {
            if (opacity > 0 && ! item)
                sourceComponent = componentTop
        }

        anchors.fill: root
        opacity: switch (pointType) {
                 case 0: return imageOpacity0
                 case 1: return imageOpacity180
                 case 2: return imageOpacity90
                 case 3: return imageOpacity270
                 }
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: componentTop

            PieceElementImage {
                source: imageName
                sourceSize: imageSourceSize
                // Don't set antialiasing, vertex antialiasing causes unwanted
                // seams between edges of the quarter squares
                antialiasing: false
                rotation: switch (pointType) {
                          case 1: return 180
                          case 2: return 270
                          case 3: return 90
                          default: return 0
                          }
                x: pointType == 1 || pointType == 3 ? -width / 2 : 0
            }
        }
    }
    Loader {
        function loadImage() {
            if (opacity > 0 && ! item)
                sourceComponent = componentBottom
        }

        anchors.fill: root
        opacity: switch (pointType) {
                 case 0: return imageOpacity180
                 case 1: return imageOpacity0
                 case 2: return imageOpacity270
                 case 3: return imageOpacity90
                 }
        onOpacityChanged: loadImage()
        Component.onCompleted: loadImage()

        Component {
            id: componentBottom

            PieceElementImage {
                source: imageNameBottom
                sourceSize: imageSourceSize
                // Don't set antialiasing, see above
                antialiasing: false
                rotation: switch (pointType) {
                          case 1: return 180
                          case 2: return 270
                          case 3: return 90
                          default: return 0
                          }
                x: pointType == 1 || pointType == 3 ? -width / 2 : 0
            }
        }
    }
}
