//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/ComputerColorDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.2

Dialog {
    id: root

    property string gameVariant
    property alias computerPlays0: checkBox0.checked
    property alias computerPlays1: checkBox1.checked
    property alias computerPlays2: checkBox2.checked
    property alias computerPlays3: checkBox3.checked

    standardButtons: StandardButton.Ok | StandardButton.Cancel

    Column {
        Label { text: "Computer plays:" }
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
}
