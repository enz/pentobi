//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "Main.js" as Logic

PentobiDialog {
    property int numberGames: ratingModel.numberGames

    footer: PentobiDialogButtonBox { ButtonClose { } }

    Item {
        implicitWidth: Math.max(Math.min(font.pixelSize * 22, maxContentWidth),
                                minContentWidth)
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
                visible: ratingModel.ratingHistory.length > 1
                Layout.fillWidth: true

                Label { text: qsTr("Recent development:") }
                RatingGraph {
                    history: ratingModel.ratingHistory
                    Layout.preferredHeight:
                        Math.min(font.pixelSize * 8,
                                 0.22 * rootWindow.contentItem.width,
                                 0.22 * rootWindow.contentItem.height)
                    Layout.fillWidth: true
                }
            }
            TableView {
                visible: ratingModel.ratingHistory.length > 0
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                model: ratingModel.tableModel
                delegate: Label {
                    font.underline: row === 0
                    text: row > 0 && column === 3 ?
                              Logic.getPlayerString(ratingModel.gameVariant,
                                                    display)
                            : display
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onClicked: menu.openMenu(row, parent)
                        onPressAndHold: menu.openMenu(row, parent)
                    }
                }
                columnSpacing: 0.4 * font.pixelSize
                rowSpacing: columnLayout.spacing
                Layout.fillWidth: true
                Layout.preferredHeight:
                    Math.min(font.pixelSize * 8,
                             0.22 * rootWindow.contentItem.width,
                             0.22 * rootWindow.contentItem.height)
                ScrollBar.vertical: ScrollBar { }
            }
            PentobiMenu {
                id: menu

                property int row

                function openMenu(row, parent) {
                    if (row < 1)
                        return
                    menu.parent = parent
                    menu.row = row
                    popup()
                }

                relativeWidth: 14

                PentobiMenuItem {
                    text: qsTr("Open Game %1").arg(
                                ratingModel.getGameNumber(menu.row - 1))
                    onTriggered: {
                        var n = ratingModel.getGameNumber(menu.row - 1)
                        Logic.openFile(ratingModel.getFile(n), "")
                        close()
                    }
                }
            }
        }
    }
}
