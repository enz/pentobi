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

    ColumnLayout {
        RowLayout {
            Layout.fillWidth: true

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
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: textFieldPlayerName0
            }
        }
        RowLayout {
            Layout.fillWidth: true

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
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: textFieldPlayerName1
            }
        }
        RowLayout {
            visible: gameVariant === "classic" || gameVariant === "trigon"
                     || gameVariant === "trigon_3"
                     || gameVariant === "classic_3"
                     || gameVariant === "nexos"
                     || gameVariant === "callisto_3"
                     || gameVariant === "callisto"
            Layout.fillWidth: true

            Label {
                text: qsTr("Player Red:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: textFieldPlayerName2
            }
        }
        RowLayout {
            visible: gameVariant === "classic" || gameVariant === "trigon"
                     || gameVariant === "nexos" || gameVariant === "callisto"
            Layout.fillWidth: true

            Label {
                text: qsTr("Player Green:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: textFieldPlayerName3
            }
        }
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Date:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: textFieldDate
            }
        }
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Time:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: textFieldTime
            }
        }
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Event:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: textFieldEvent
            }
        }
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Round:")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
            }
            TextField {
                id: textFieldRound
            }
        }
    }
}
