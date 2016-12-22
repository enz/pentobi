import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls 1.1 as Controls1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0
import "Main.js" as Logic

Dialog {
    title: qsTr("Rating")
    onVisibleChanged: if (! visible) gameDisplay.forceActiveFocus() // QTBUG-48456

    ColumnLayout
    {
        GridLayout {
            columns: 2

            Label { text: qsTr("Your rating:") }
            Label {
                text: ratingModel.numberGames === 0 ?
                          "--" : Math.round(ratingModel.rating).toString()
                Layout.fillWidth: true
                font.bold: true
            }
            Label { text: qsTr("Game variant:") }
            Label {
                text: switch (ratingModel.gameVariant) {
                      case "classic_2": return qsTr("Classic (2 players)")
                      case "classic_3": return qsTr("Classic (3 players)")
                      case "classic": return qsTr("Classic (4 players)")
                      case "duo": return qsTr("Duo")
                      case "junior": return qsTr("Junior")
                      case "trigon_2": return qsTr("Trigon (2 players)")
                      case "trigon_3": return qsTr("Trigon (3 players)")
                      case "trigon": return qsTr("Trigon (4 players)")
                      case "nexos_2": return qsTr("Nexos (2 players)")
                      case "nexos": return qsTr("Nexos (4 players)")
                      case "callisto_2": return qsTr("Callisto (2 players)")
                      case "callisto_3": return qsTr("Callisto (3 players)")
                      case "callisto": return qsTr("Callisto (4 players)")
                      default: return ""
                      }
                Layout.fillWidth: true
            }
            Label { text: qsTr("Rated games:") }
            Label {
                text: ratingModel.numberGames
                Layout.fillWidth: true
            }
            Label { text: qsTr("Best previous rating:") }
            Label {
                text: ratingModel.numberGames === 0 ?
                          "--" : Math.round(ratingModel.bestRating).toString()
                Layout.fillWidth: true
            }
        }
        Controls1.Button {
            visible: ratingModel.numberGames > 0
            text: qsTr("Clear rating")
            onClicked: Logic.clearRating()
        }
    }
}
