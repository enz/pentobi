//-----------------------------------------------------------------------------
/** @file pentobi/PentobiBusyIndicator.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls

BusyIndicator {
    id: root

    contentItem: Item {
        implicitWidth: 64
        implicitHeight: 64

        Item {
            id: item

            anchors.fill: parent
            opacity: root.running ? 1 : 0

            Repeater {
                id: repeater

                model: 8

                Rectangle {
                    x: item.width / 2 - width / 2
                    y: item.height / 2 - height / 2
                    width: 0.15 * item.width; height: width
                    radius: width / 2
                    color: theme.colorText
                    opacity: 0.5
                    transform: [
                        Translate {
                            y: -item.width / 2 + width
                        },
                        Rotation {
                            angle: index / repeater.count * 360
                            origin { x: width / 2; y: height / 2 }
                        }
                    ]
                }
            }
            RotationAnimator {
                target: item
                running: root.visible && root.running
                from: 0; to: 360
                loops: Animation.Infinite
                duration: 1700
            }
        }
    }
}
