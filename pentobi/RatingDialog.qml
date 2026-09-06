//-----------------------------------------------------------------------------
/** @file pentobi/RatingDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "main.js" as Logic

PentobiDialog {
    property int numberGames: ratingModel.numberGames

    function openSelectedGame() {
        var row = selectionModel.currentIndex.row
        if (row < 1)
            return
        var n = ratingModel.getGameNumber(row - 1)
        Logic.openFile(ratingModel.getFile(n), "")
    }

    footer: DialogButtonBox {
        defaultButton: buttonClose

        Button {
            id: buttonOpen

            enabled: selectionModel.currentIndex.row > 0
            text: qsTr("Open Game")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            focusPolicy: workaroundOskBug ? Qt.NoFocus : Qt.StrongFocus
            onClicked: openSelectedGame()
        }
        ButtonClose {
            id: buttonClose
        }
    }

    Item {
        implicitWidth: Math.max(Math.min(font.pixelSize * 28, maxContentWidth),
                                minContentWidth)
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout
        {
            id: columnLayout

            anchors.fill: parent

            GridLayout {
                columns: 2

                Label { text: qsTr("Your rating:") }
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
            HorizontalHeaderView {
                id: headerView

                syncView: tableView
                model: [
                    //: Table header for game number in rating dialog
                    qsTr("Game"),
                    //: Table header for game result in rating dialog
                    qsTr("Result"),
                    //: Table header for level in rating dialog
                    qsTr("Level"),
                    //: Table header for player color(s) in rating dialog
                    qsTr("Your Color"),
                    //: Table header for game date in rating dialog
                    qsTr("Date")
                ]
                delegate: Label {
                    text: modelData
                    font.underline: true
                }
                Layout.fillWidth: true
            }
            TableView {
                id: tableView

                visible: ratingModel.ratingHistory.length > 0
                focus: true
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                model: ratingModel.tableModel
                columnWidthProvider:
                    column => {
                        switch (column) {
                            case 0: return font.pixelSize * 4
                            case 1: return font.pixelSize * 6
                            case 2: return font.pixelSize * 5
                            case 3: return font.pixelSize * 7
                            case 4: return font.pixelSize * 6
                        }
                    }
                selectionModel: ItemSelectionModel {
                    id: selectionModel

                    model: ratingModel.tableModel
                }
                selectionBehavior: TableView.SelectRows
                selectionMode: TableView.SingleSelection
                editTriggers: TableView.NoEditTriggers
                delegate: TableViewDelegate {
                    rightPadding: columnLayout.spacing
                    topPadding: columnLayout.spacing / 2
                    bottomPadding: columnLayout.spacing / 2
                    contentItem: Label {
                        text: column === 3 ?
                                  Logic.getPlayerString(ratingModel.gameVariant,
                                                        model.display)
                                : model.display
                        horizontalAlignment: column === 2 ? Text.AlignHCenter : Text.AlignLeft
                    }
                    background: Rectangle {
                        visible: selectionModel.currentIndex.row === row
                        color: "#888888"
                    }

                    TapHandler {
                        gesturePolicy: TapHandler.ReleaseWithinBounds
                        onTapped: {
                            let modelIndex = tableView.model.index(row, column)
                            selectionModel.setCurrentIndex(
                                        modelIndex, 
                                        ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows)
                        }
                    }
                }
                Keys.onPressed:
                    event => {
                        if (event.key === Qt.Key_Space) {
                            buttonOpen.click()
                            event.accepted = true
                        }
                        else if (event.key === Qt.Key_Tab) {
                            selectionModel.clearCurrentIndex()
                            buttonClose.forceActiveFocus()
                            event.accepted = true
                        }
                    }
                ScrollBar.vertical: ScrollBar { }
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(font.pixelSize * 10)
            }
        }
    }
}
