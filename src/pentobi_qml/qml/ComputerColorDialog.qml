import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

Dialog {
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

        ColumnLayout {
            CheckBox {
                id: checkBox0

                enabled: ! isRated
                text: {
                    switch (gameVariant) {
                    case "classic_2":
                    case "trigon_2":
                    case "nexos_2":
                        return qsTr("Blue/Red")
                    default:
                        qsTr("Blue")
                    }
                }
                onClicked: {
                    if (gameVariant == "classic_2" || gameVariant == "trigon_2"
                            || gameVariant == "nexos_2")
                        computerPlays2 = checked
                }
            }
            CheckBox {
                id: checkBox1

                enabled: ! isRated
                text: {
                    switch (gameVariant) {
                    case "classic_2":
                    case "trigon_2":
                    case "nexos_2":
                        return qsTr("Yellow/Green")
                    case "duo":
                    case "junior":
                    case "callisto_2":
                        return qsTr("Green")
                    default:
                        qsTr("Yellow")
                    }
                }
                onClicked: {
                    if (gameVariant == "classic_2" || gameVariant == "trigon_2"
                            || gameVariant == "nexos_2")
                        computerPlays3 = checked
                }
            }
            CheckBox {
                id: checkBox2

                enabled: ! isRated
                text: qsTr("Red")
                visible: gameVariant == "classic" || gameVariant == "trigon"
                         || gameVariant == "trigon_3"
                         || gameVariant == "classic_3"
                         || gameVariant == "nexos"
                         || gameVariant == "callisto_3"
                         || gameVariant == "callisto"
            }
            CheckBox {
                id: checkBox3

                enabled: ! isRated
                text: qsTr("Green")
                visible: gameVariant == "classic" || gameVariant == "trigon"
                         || gameVariant == "nexos" || gameVariant == "callisto"
            }
        }
    }
}
