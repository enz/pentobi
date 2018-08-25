//-----------------------------------------------------------------------------
/** @file pentobi/qml/QuarterSquare.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// Piece element used in GembloQ. See Square.qml for comments
Item {
    property int pointType

    Loader {
        opacity: switch (pointType) {
                 case 0: return imageOpacity0
                 case 1: return imageOpacity180
                 case 2: return imageOpacity90
                 case 3: return imageOpacity270
                 }
        sourceComponent: opacity > 0 || item ? componentTop : null

        Component {
            id: componentTop

            Image {
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
        opacity: switch (pointType) {
                 case 0: return imageOpacitySmall0
                 case 1: return imageOpacitySmall180
                 case 2: return imageOpacitySmall90
                 case 3: return imageOpacitySmall270
                 }
        sourceComponent: opacity > 0 || item ? componentSmallTop : null

        Component {
            id: componentSmallTop

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
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
    Loader {
        opacity: switch (pointType) {
                 case 0: return imageOpacity180
                 case 1: return imageOpacity0
                 case 2: return imageOpacity270
                 case 3: return imageOpacity90
                 }
        sourceComponent: opacity > 0 || item ? componentBottom : null

        Component {
            id: componentBottom

            Image {
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
    Loader {
        opacity: switch (pointType) {
                 case 0: return imageOpacitySmall180
                 case 1: return imageOpacitySmall0
                 case 2: return imageOpacitySmall270
                 case 3: return imageOpacitySmall90
                 }
        sourceComponent: opacity > 0 || item ? componentSmallBottom : null

        Component {
            id: componentSmallBottom

            Image {
                source: imageNameBottom
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
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
