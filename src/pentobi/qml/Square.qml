//-----------------------------------------------------------------------------
/** @file pentobi/qml/Square.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// Piece element (square) with pseudo-3D effect.
// Simulates lighting by using different images for the lighting at different
// rotations and interpolating between them with an opacity animation. All
// images have two versions with the sourceSize optimzed for the statically
// displayed states on the board and in the piece selector, which produces
// better results than using Image.mipmap and avoids a mipmap bug with Nvidia
// cards (QTBUG-57845).
Item {
    id: root

    Loader {
        opacity: imageOpacity0
        sourceComponent: opacity > 0 || item ? component0 : null

        Component {
            id: component0

            Image {
                source: imageName
                sourceSize: imageSourceSize
                antialiasing: true
            }
        }
    }
    Loader {
        opacity: imageOpacitySmall0
        sourceComponent: opacity > 0 || item ? componentSmall0 : null

        Component {
            id: componentSmall0

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                antialiasing: true
            }
        }
    }
    Loader {
        opacity: imageOpacity90
        sourceComponent: opacity > 0 || item ? component90 : null

        Component {
            id: component90

            Image {
                source: imageName
                sourceSize: imageSourceSize
                antialiasing: true
                rotation: -90
            }
        }
    }
    Loader {
        opacity: imageOpacitySmall90
        sourceComponent: opacity > 0 || item ? componentSmall90 : null

        Component {
            id: componentSmall90

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                antialiasing: true
                rotation: -90
            }
        }
    }
    Loader {
        opacity: imageOpacity180
        sourceComponent: opacity > 0 || item ? component180 : null

        Component {
            id: component180

            Image {
                source: imageName
                sourceSize: imageSourceSize
                antialiasing: true
                rotation: -180
            }
        }
    }
    Loader {
        opacity: imageOpacitySmall180
        sourceComponent: opacity > 0 || item ? componentSmall180 : null

        Component {
            id: componentSmall180

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                antialiasing: true
                rotation: -180
            }
        }
    }
    Loader {
        opacity: imageOpacity270
        sourceComponent: opacity > 0 || item ? component270 : null

        Component {
            id: component270

            Image {
                source: imageName
                sourceSize: imageSourceSize
                antialiasing: true
                rotation: -270
            }
        }
    }
    Loader {
        opacity: imageOpacitySmall270
        sourceComponent: opacity > 0 || item ? componentSmall270 : null

        Component {
            id: componentSmall270

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                antialiasing: true
                rotation: -270
            }
        }
    }
}
