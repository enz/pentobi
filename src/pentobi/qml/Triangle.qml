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

            PieceElementImage {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                antialiasing: true
            }
        }
    }
    Loader {
        opacity: imageOpacity60
        sourceComponent: opacity > 0 || item ? component60 : null

        Component {
            id: component60

            PieceElementImage {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
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

            PieceElementImage {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
                antialiasing: true
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

            PieceElementImage {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
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

            PieceElementImage {
                source: isDownward ? imageNameDownward : imageName
                sourceSize: imageSourceSize
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

            PieceElementImage {
                source: isDownward ? imageName : imageNameDownward
                sourceSize: imageSourceSize
                antialiasing: true
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
