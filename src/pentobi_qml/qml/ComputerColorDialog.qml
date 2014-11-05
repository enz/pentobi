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

    title: qsTr("Computer Colors")
    standardButtons: StandardButton.Ok | StandardButton.Cancel

    GroupBox {
        title: qsTr("Computer plays:")
        flat: true

        Column {
            CheckBox {
                id: checkBox0

                text: {
                    switch (gameVariant) {
                    case "classic_2":
                    case "trigon_2":
                        return qsTr("Blue/Red")
                    default:
                        qsTr("Blue")
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
                        return qsTr("Yellow/Green")
                    case "duo":
                    case "junior":
                        return qsTr("Green")
                    default:
                        qsTr("Yellow")
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

                text: qsTr("Red")
                visible: gameVariant == "classic" ||
                         gameVariant == "trigon" ||
                         gameVariant == "trigon_3" ||
                         gameVariant == "classic_3"
            }
            CheckBox {
                id: checkBox3

                text: qsTr("Green")
                visible: gameVariant == "classic" || gameVariant == "trigon"
            }
        }
    }
}
