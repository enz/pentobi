import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import "Main.js" as Logic

Dialog {
    property int numberGames: ratingModel.numberGames
    property var history: ratingModel.history

    //: Window title for the rating dialog.
    title: isAndroid ? "" : qsTr("Rating")
    x: (rootWindow.width - width) / 2
    y: (rootWindow.height - height) / 2
    parent: ApplicationWindow.overlay
    standardButtons: Dialog.Ok
    onVisibleChanged:
        if (! visible) {
            // See comment in Main.qml at ratingModel.onHistoryChanged
            close()
        }

    Column
    {
        spacing: Screen.pixelDensity * 2

        GridLayout {
            columns: 2

            Label {
                id: labelYourRating

                text: qsTr("Your rating:")
            }
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
                      case "callisto_2_4": return qsTr("Callisto (2 players, 4 colors)")
                      case "callisto_3": return qsTr("Callisto (3 players)")
                      case "callisto": return qsTr("Callisto (4 players)")
                      case "gembloq": return qsTr("GembloQ (4 players)")
                      case "gembloq_2": return qsTr("GembloQ (2 players, 2 colors)")
                      case "gembloq_2_4": return qsTr("GembloQ (2 players, 4 colors)")
                      case "gembloq_3": return qsTr("GembloQ (3 players)")
                      default: return ""
                      }
                Layout.fillWidth: true
            }
            Label { text: qsTr("Rated games:") }
            Label {
                text: numberGames
                Layout.fillWidth: true
            }
            Label { text: qsTr("Best previous rating:") }
            Label {
                text: numberGames < 2 ? "--" : Math.round(ratingModel.bestRating).toString()
                Layout.fillWidth: true
            }
        }

        Column {
            Label {
                visible: history.length > 1
                text: qsTr("Recent development:")
                Layout.topMargin: Math.round(Screen.pixelDensity)
            }
            RatingGraph {
                visible: history.length > 1
                history: ratingModel.history
                implicitWidth: Math.min(labelYourRating.font.pixelSize * 25, 0.9 * rootWindow.width)
                implicitHeight: implicitWidth / 3
            }
        }

        ScrollView
        {
            visible: history.length > 0
            implicitWidth: Math.min(labelYourRating.font.pixelSize * 25, 0.9 * rootWindow.width)
            implicitHeight: implicitWidth / 3
            clip: true

            Item
            {
                implicitHeight: grid.height
                implicitWidth: grid.width

                GridLayout {
                    id: grid

                    rows: history.length + 1
                    flow: Grid.TopToBottom

                    Label {
                        id: gameHeader

                        font.underline: true
                        text: qsTr("Game")
                    }
                    Repeater {
                        id: gameRepeater

                        model: history

                        Label { text: modelData.number }
                    }
                    Label { font.underline: true; text: qsTr("Result") }
                    Repeater {
                        model: history

                        Label {
                            text: switch (modelData.result) {
                                  case 1:
                                      //: Result of rated game is a win
                                      return qsTr("Win")
                                  case 0:
                                      //: Result of rated game is a loss
                                      return qsTr("Loss")
                                  case 0.5:
                                      //: Result of rated game is a tie. Abbreviate long translations to
                                      //: ensure that all columns of rated games list are visible on
                                      //: mobile devices with small screens.
                                      return qsTr("Tie")
                                  }
                        }
                    }
                    Label { font.underline: true; text: qsTr("Level") }
                    Repeater {
                        model: history

                        Label { text: modelData.level }
                    }
                    Label { font.underline: true; text: qsTr("Your Color") }
                    Repeater {
                        model: history

                        Label { text: gameModel.getPlayerString(modelData.color) }
                    }
                    Label { font.underline: true; text: qsTr("Date") }
                    Repeater {
                        model: history

                        Label { text: modelData.date }
                    }
                }
                MouseArea {
                    function openMenu(x, y) {
                        if (y < gameHeader.height)
                            return
                        var n = history.length
                        var i
                        for (i = 1; i < n; ++i)
                            if (y < gameRepeater.itemAt(i).y)
                                break
                        menu.row = i - 1
                        menu.popup()
                    }

                    anchors.fill: grid
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: openMenu(mouseX, mouseY)
                    onPressAndHold: openMenu(mouseX, mouseY)
                }
            }
        }
        Menu {
            id: menu

            property int row

            MenuItem {
                text: history && menu.row < history.length ?
                          qsTr("Open Game %1").arg(history[menu.row].number) : ""
                onTriggered: {
                    queuedOpenRatedGame.byteArray = history[menu.row].sgf
                    queuedOpenRatedGame.restart()
                    // See comment in Main.qml at ratingModel.onHistoryChanged
                    close()
                }
            }
        }
    }
}
