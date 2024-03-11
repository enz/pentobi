//-----------------------------------------------------------------------------
/** @file pentobi/LineSegment.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick

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

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize: imageSourceSize
                antialiasing: true
                mirror: ! isHorizontal
                rotation: isHorizontal ? 0 : -90
            }
        }
    }
    Loader {
        anchors.fill: root
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

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize: imageSourceSize
                antialiasing: true
                mirror: isHorizontal
                rotation: isHorizontal ? -180 : -90
            }
        }
    }
    Loader {
        anchors.fill: root
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

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize: imageSourceSize
                antialiasing: true
                mirror: ! isHorizontal
                rotation: isHorizontal ? -180 : -270
            }
        }
    }
    Loader {
        anchors.fill: root
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

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize: imageSourceSize
                antialiasing: true
                mirror: isHorizontal
                rotation: isHorizontal ? 0 : -270
            }
        }
    }
    Loader {
        anchors.fill: root
        opacity: imageOpacitySmall270
        sourceComponent: opacity > 0 || item ? componentSmall270 : null

        Component {
            id: componentSmall270

            Image {
                source: imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                antialiasing: true
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                mirror: isHorizontal
                rotation: isHorizontal ? 0 : -270
            }
        }
    }
}
