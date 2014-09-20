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
    property bool computerPlays0
    property bool computerPlays1
    property bool computerPlays2
    property bool computerPlays3

    property real _margin: label.height

    signal okClicked()

    visible: false
    color: palette.window
    width: column.implicitWidth + 2 * _margin
    height: column.implicitHeight + 2 * _margin
    radius: _margin / 2

    SystemPalette { id: palette }

    Column {
        id: column

        anchors.centerIn: root
        spacing: label.height / 2

        Label {
            id: label
            anchors.horizontalCenter: column.horizontalCenter
            text: "Colors played by the computer:"
        }

        Column {
            anchors.horizontalCenter: column.horizontalCenter
            CheckBox {
                text: {
                    switch (gameVariant) {
                    case "classic_2":
                    case "trigon_2":
                        return "Blue/Red"
                    default:
                        "Blue"
                    }
                }
                checked: computerPlays0
                onClicked: {
                    computerPlays0 = checked
                    if (gameVariant == "classic_2" || gameVariant == "trigon_2")
                        computerPlays2 = checked
                }
            }
            CheckBox {
                text: {
                    switch (gameVariant) {
                    case "classic_2":
                    case "trigon_2":
                        return "Yellow/Green"
                    case "duo":
                        return "Green"
                    default:
                        "Yellow"
                    }
                }
                checked: computerPlays1
                onClicked: {
                    computerPlays1 = checked
                    if (gameVariant == "classic_2" || gameVariant == "trigon_2")
                        computerPlays3 = checked
                }
            }
            CheckBox {
                text: "Green"
                visible: gameVariant == "classic" || gameVariant == "trigon"
                checked: computerPlays2
                onClicked: computerPlays2 = checked
            }
            CheckBox {
                text: "Red"
                visible: gameVariant == "classic" ||
                         gameVariant == "trigon" ||
                         gameVariant == "trigon_3"
                checked: computerPlays3
                onClicked: computerPlays3 = checked
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
