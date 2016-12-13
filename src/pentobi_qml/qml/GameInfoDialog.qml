import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0

Dialog {
    property alias playerName0: textFieldPlayerName0.text
    property alias playerName1: textFieldPlayerName1.text
    property alias playerName2: textFieldPlayerName2.text
    property alias playerName3: textFieldPlayerName3.text
    property alias date: textFieldDate.text
    property alias time: textFieldTime.text
    property alias event: textFieldEvent.text
    property alias round: textFieldRound.text
    property string gameVariant: gameModel.gameVariant

    title: qsTr("Game Info")
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    onAccepted: {
        gameModel.playerName0 = playerName0
        gameModel.playerName1 = playerName1
        gameModel.playerName2 = playerName2
        gameModel.playerName3 = playerName3
        gameModel.date = date
        gameModel.time = time
        gameModel.event = event
        gameModel.round = round
        gameDisplay.forceActiveFocus() // QTBUG-48456
    }
    onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456

    GridLayout {
        columns: 2

        Label {
            text: {
                switch (gameModel.gameVariant) {
                case "classic_2":
                case "trigon_2":
                case "nexos_2":
                    return qsTr("Player Blue/Red:")
                default:
                    qsTr("Player Blue:")
                }
            }
        }
        TextField {
            id: textFieldPlayerName0

            Layout.fillWidth: true
        }
        Label {
            text: {
                switch (gameVariant) {
                case "classic_2":
                case "trigon_2":
                case "nexos_2":
                    return qsTr("Player Yellow/Green:")
                case "duo":
                case "junior":
                case "callisto_2":
                    return qsTr("Player Green:")
                default:
                    qsTr("Player Yellow:")
                }
            }
        }
        TextField {
            id: textFieldPlayerName1

            Layout.fillWidth: true
        }
        Label {
            visible: textFieldPlayerName2.visible
            text: qsTr("Player Red:")
        }
        TextField {
            id: textFieldPlayerName2

            visible: gameVariant === "classic" || gameVariant === "trigon"
                     || gameVariant === "trigon_3"
                     || gameVariant === "classic_3"
                     || gameVariant === "nexos"
                     || gameVariant === "callisto_3"
                     || gameVariant === "callisto"

            Layout.fillWidth: true
        }
        Label {
            visible: textFieldPlayerName3.visible
            text: qsTr("Player Green:")
        }
        TextField {
            id: textFieldPlayerName3

            visible: gameVariant === "classic" || gameVariant === "trigon"
                     || gameVariant === "nexos" || gameVariant === "callisto"
            Layout.fillWidth: true
        }
        Label { text: qsTr("Date:") }
        TextField {
            id: textFieldDate

            Layout.fillWidth: true
        }
        Label { text: qsTr("Time:") }
        TextField {
            id: textFieldTime

            Layout.fillWidth: true
        }
        Label { text: qsTr("Event:") }
        TextField {
            id: textFieldEvent

            Layout.fillWidth: true
        }
        Label { text: qsTr("Round:") }
        TextField {
            id: textFieldRound

            Layout.fillWidth: true
        }
    }
}
