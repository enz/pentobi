import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

Pentobi.Dialog {
    property int numberGames: ratingModel.numberGames
    property var history: ratingModel.history

    //: Window title for the rating dialog.
    title: isDesktop ? qsTr("Rating") : ""
    footer: OkButton { }

    Item {
        // Make it wide enough to show a the graph
        implicitWidth: Math.min(font.pixelSize * 30, 0.9 * rootWindow.width)
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout
        {
            id: columnLayout

            anchors.fill: parent

            GridLayout {
                columns: 2

                Label {
                    id: labelYourRating

                    text: qsTr("Your rating:")
                }
                Label {
                    text: ratingModel.numberGames === 0 ?
                              "" : Math.round(ratingModel.rating).toString()
                    Layout.fillWidth: true
                    font.bold: true
                }
                Label { text: qsTr("Game variant:") }
                Label {
                    text: switch (ratingModel.gameVariant) {
                          case "classic_2":
                              //: Short for Classic (2 players)
                              return qsTr("Classic (2)")
                          case "classic_3":
                              //: Short for Classic (3 players)
                              return qsTr("Classic (3)")
                          case "classic":
                              //: Short for Classic (4 players)
                              return qsTr("Classic (4)")
                          case "duo":
                              return qsTr("Duo")
                          case "junior":
                              return qsTr("Junior")
                          case "trigon_2":
                              //: Short for Trigon (2 players)
                              return qsTr("Trigon (2)")
                          case "trigon_3":
                              //: Short for Trigon (3 players)
                              return qsTr("Trigon (3)")
                          case "trigon":
                              //: Short for Trigon (4 players)
                              return qsTr("Trigon (4)")
                          case "nexos_2":
                              //: Short for Nexos (2 players)
                              return qsTr("Nexos (2)")
                          case "nexos":
                              //: Short for Nexos (4 players)
                              return qsTr("Nexos (4)")
                          case "callisto_2":
                              //: Short for Callisto (2 players, 2 colors)
                              return qsTr("Callisto (2)")
                          case "callisto_2_4":
                              //: Short for Callisto (2 players, 4 colors)
                              return qsTr("Callisto (2/4)")
                          case "callisto_3":
                              //: Short for Callisto (3 players)
                              return qsTr("Callisto (3)")
                          case "callisto":
                              //: Short for Callisto (4 players)
                              return qsTr("Callisto (4)")
                          case "gembloq":
                              //: Short for GembloQ (4 players)
                              return qsTr("GembloQ (4)")
                          case "gembloq_2":
                              //: Short for GembloQ (2 players, 2 colors)
                              return qsTr("GembloQ (2)")
                          case "gembloq_2_4":
                              //: Short for GembloQ (2 players, 4 colors)
                              return qsTr("GembloQ (2/4)")
                          case "gembloq_3":
                              //: Short for GembloQ (3 players)
                              return qsTr("GembloQ (3)")
                          default: return ""
                          }
                    Layout.fillWidth: true
                }
                Label { text: qsTr("Rated games:") }
                Label {
                    text: numberGames
                    Layout.fillWidth: true
                }
                Label {
                    visible: numberGames > 1
                    text: qsTr("Best previous rating:")
                }
                Label {
                    visible: numberGames > 1
                    text: Math.round(ratingModel.bestRating).toString()
                    Layout.fillWidth: true
                }
            }
            ColumnLayout {
                Layout.fillWidth: true

                Label {
                    visible: history.length > 1
                    text: qsTr("Recent development:")
                }
                RatingGraph {
                    visible: history.length > 1
                    history: ratingModel.history
                    Layout.preferredHeight:
                        Math.min(Math.min(font.pixelSize * 8, 0.22 * rootWindow.width),
                                 0.22 * rootWindow.height)
                    Layout.fillWidth: true
                }
            }
            ScrollView
            {
                visible: history.length > 0
                clip: true
                Layout.fillWidth: true
                Layout.preferredHeight:
                    Math.min(Math.min(font.pixelSize * 8, 0.22 * rootWindow.width),
                             0.22 * rootWindow.height)

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
                            menu.popup(mouseX, mouseY)
                        }

                        anchors.fill: grid
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onClicked: openMenu(mouseX, mouseY)
                        onPressAndHold: openMenu(mouseX, mouseY)

                        Menu {
                            id: menu

                            property int row

                            Pentobi.MenuItem {
                                text: history && menu.row < history.length ?
                                          qsTr("Open Game %1").arg(history[menu.row].number) : ""
                                onTriggered: {
                                    queuedOpenRatedGame.byteArray = history[menu.row].sgf
                                    queuedOpenRatedGame.restart()
                                    close()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
