//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/ComputerColorDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: root

    property string gameVariant
    property alias computerPlays0: checkBox0.checked
    property alias computerPlays1: checkBox1.checked
    property alias computerPlays2: checkBox2.checked
    property alias computerPlays3: checkBox3.checked

    property real _margin: label.height

    signal okClicked()

    visible: false
    color: palette.window
    width: column.implicitWidth + 2 * _margin
    height: column.implicitHeight + 2 * _margin
    radius: 0.2 * _margin

    SystemPalette { id: palette }

    Column {
        id: column

        anchors.centerIn: root
        spacing: 0.5 * label.height

        Label {
            id: label
            anchors.horizontalCenter: column.horizontalCenter
            text: "Colors played by the computer:"
        }

        Column {
            anchors.horizontalCenter: column.horizontalCenter
            spacing: 0.3 * label.height

            CheckBox {
                id: checkBox0

                text: {
                    switch (gameVariant) {
                    case "classic_2":
                    case "trigon_2":
                        return "Blue/Red"
                    default:
                        "Blue"
                    }
                }
                onClicked: {
                    if (gameVariant == "classic_2"
                            || gameVariant == "trigon_2")
                        computerPlays2 = checked
                }
            }
            CheckBox {
                id: checkBox1

                text: {
                    switch (gameVariant) {
                    case "classic_2":
                    case "trigon_2":
                        return "Yellow/Green"
                    case "duo":
                    case "junior":
                        return "Green"
                    default:
                        "Yellow"
                    }
                }
                onClicked: {
                    if (gameVariant == "classic_2" ||
                            gameVariant == "trigon_2")
                        computerPlays3 = checked
                }
            }
            CheckBox {
                id: checkBox2

                text: "Red"
                visible: gameVariant == "classic" ||
                         gameVariant == "trigon" ||
                         gameVariant == "trigon_3"
            }
            CheckBox {
                id: checkBox3

                text: "Green"
                visible: gameVariant == "classic" || gameVariant == "trigon"
            }
        }

        Button {
            text: "Ok"
            anchors.horizontalCenter: column.horizontalCenter
            onClicked: {
                okClicked()
                root.visible = false
            }
        }
    }
}
