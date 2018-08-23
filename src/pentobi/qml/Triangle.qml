//-----------------------------------------------------------------------------
/** @file pentobi/qml/Triangle.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.3

// Piece element used in Trigon. See Square.qml for comments
Item {
    property bool isDownward

    Loader {
        opacity: imageOpacity0
        sourceComponent: opacity > 0 || item ? component0 : null

        Component {
            id: component0

            Image {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                smooth: false
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
                source: isDownward ? imageNameDownward : imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                smooth: false
                antialiasing: true
            }
        }
    }
    Loader {
        opacity: imageOpacity60
        sourceComponent: opacity > 0 || item ? component60 : null

        Component {
            id: component60

            Image {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                smooth: false
                antialiasing: true
                transform: [
                    Rotation {
                        angle: -60
                        origin {
                            x: width / 2
                            y: isDownward ? 2 * height / 3 : height / 3
                        }
                    },
                    Translate { y:  isDownward ? -height / 3 : height / 3 }
                ]
            }
        }
    }
    Loader {
        opacity: imageOpacitySmall60
        sourceComponent: opacity > 0 || item ? componentSmall60 : null

        Component {
            id: componentSmall60

            Image {
                source: isDownward ? imageName : imageNameDownward
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                smooth: false
                antialiasing: true
                transform: [
                    Rotation {
                        angle: -60
                        origin {
                            x: width / 2
                            y: isDownward ? 2 * height / 3 : height / 3
                        }
                    },
                    Translate { y:  isDownward ? -height / 3 : height / 3 }
                ]
            }
        }
    }
    Loader {
        opacity: imageOpacity120
        sourceComponent: opacity > 0 || item ? component120 : null

        Component {
            id: component120

            Image {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                antialiasing: true
                smooth: false
                transform: Rotation {
                    angle: -120
                    origin {
                        x: width / 2
                        y: isDownward ? height / 3 : 2 * height / 3
                    }
                }
            }
        }
    }
    Loader {
        opacity: imageOpacitySmall120
        sourceComponent: opacity > 0 || item ? componentSmall120 : null

        Component {
            id: componentSmall120

            Image {
                source: isDownward ? imageNameDownward : imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                antialiasing: true
                smooth: false
                transform: Rotation {
                    angle: -120
                    origin {
                        x: width / 2
                        y: isDownward ? height / 3 : 2 * height / 3
                    }
                }
            }
        }
    }
    Loader {
        opacity: imageOpacity180
        sourceComponent: opacity > 0 || item ? component180 : null

        Component {
            id: component180

            Image {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                smooth: false
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
                source: isDownward ? imageName : imageNameDownward
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                smooth: false
                antialiasing: true
                rotation: -180
            }
        }
    }
    Loader {
        opacity: imageOpacity240
        sourceComponent: opacity > 0 || item ? component240 : null

        Component {
            id: component240

            Image {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                smooth: false
                antialiasing: true
                transform: Rotation {
                    angle: -240
                    origin {
                        x: width / 2
                        y: isDownward ? height / 3 : 2 * height / 3
                    }
                }
            }
        }
    }
    Loader {
        opacity: imageOpacitySmall240
        sourceComponent: opacity > 0 || item ? componentSmall240 : null

        Component {
            id: componentSmall240

            Image {
                source: isDownward ? imageNameDownward : imageName
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                smooth: false
                antialiasing: true
                transform: Rotation {
                    angle: -240
                    origin {
                        x: width / 2
                        y: isDownward ? height / 3 : 2 * height / 3
                    }
                }
            }
        }
    }
    Loader {
        opacity: imageOpacity300
        sourceComponent: opacity > 0 || item ? component300 : null

        Component {
            id: component300

            Image {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                antialiasing: true
                smooth: false
                transform: [
                    Rotation {
                        angle: -300
                        origin {
                            x: width / 2
                            y: isDownward ? 2 * height / 3 : height / 3
                        }
                    },
                    Translate { y:  isDownward ? -height / 3 : height / 3 }
                ]
            }
        }
    }
    Loader {
        opacity: imageOpacitySmall300
        sourceComponent: opacity > 0 || item ? componentSmall300 : null

        Component {
            id: componentSmall300

            Image {
                source: isDownward ? imageName : imageNameDownward
                width: imageSourceSize.width
                height: imageSourceSize.height
                sourceSize {
                    width: scaleUnplayed * imageSourceSize.width
                    height: scaleUnplayed * imageSourceSize.height
                }
                antialiasing: true
                smooth: false
                transform: [
                    Rotation {
                        angle: -300
                        origin {
                            x: width / 2
                            y: isDownward ? 2 * height / 3 : height / 3
                        }
                    },
                    Translate { y:  isDownward ? -height / 3 : height / 3 }
                ]
            }
        }
    }
}
