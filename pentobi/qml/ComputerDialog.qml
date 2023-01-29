//-----------------------------------------------------------------------------
/** @file pentobi/qml/ComputerDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Main.js" as Logic

PentobiDialog {
    id: root

    footer: DialogButtonBoxOkCancel { }
    onOpened: {
        checkBox0.checked = computerPlays0
        checkBox1.checked = computerPlays1
        checkBox2.checked = computerPlays2
        checkBox3.checked = computerPlays3
        slider.value = playerModel.level
    }
    onAccepted: {
        computerPlays0 = checkBox0.checked
        computerPlays1 = checkBox1.checked
        computerPlays2 = checkBox2.checked
        computerPlays3 = checkBox3.checked
        if (! Logic.isComputerToPlay() || playerModel.level !== slider.value)
            Logic.cancelRunning()
        playerModel.level = slider.value
        if (! gameModel.isGameOver)
            Logic.checkComputerMove()
    }

    Item {
        implicitWidth:
            Math.max(Math.min(font.pixelSize * 16, maxContentWidth),
                     columnLayout.implicitWidth, minContentWidth)
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            ColumnLayout {
                Layout.fillWidth: true

                Label { text: qsTr("Computer plays:") }
                GridLayout {
                    columns: gameModel.nuPlayers === 2 ? 1 : 2
                    Layout.fillWidth: true

                    Row {
                        Layout.fillWidth: true

                        Rectangle {
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameView.color0[0]
                        }
                        Rectangle {
                            visible: gameModel.nuColors === 4
                                     && gameModel.nuPlayers === 2
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameView.color2[0]
                        }
                        Switch {
                            id: checkBox0

                            enabled: ! isRated
                            text:
                                Logic.getPlayerString(gameModel.gameVariant, 0)
                            onClicked:
                                if (gameModel.nuColors === 4
                                        && gameModel.nuPlayers === 2)
                                    checkBox2.checked = checked
                        }
                    }
                    Row {
                        Layout.fillWidth: true

                        Rectangle {
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameView.color1[0]
                        }
                        Rectangle {
                            visible: gameModel.nuColors === 4
                                     && gameModel.nuPlayers === 2
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameView.color3[0]
                        }
                        Switch {
                            id: checkBox1

                            enabled: ! isRated
                            text:
                                Logic.getPlayerString(gameModel.gameVariant, 1)
                            onClicked:
                                if (gameModel.nuColors === 4
                                        && gameModel.nuPlayers === 2)
                                    checkBox3.checked = checked
                        }
                    }
                    Row {
                        visible: gameModel.nuPlayers > 3
                        Layout.fillWidth: true

                        Rectangle {
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameView.color3[0]
                        }
                        Switch {
                            id: checkBox3

                            enabled: ! isRated
                            text:
                                Logic.getPlayerString(gameModel.gameVariant, 3)
                        }
                    }
                    Row {
                        visible: gameModel.nuPlayers > 2
                        Layout.fillWidth: true

                        Rectangle {
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameView.color2[0]
                        }
                        Switch {
                            id: checkBox2

                            enabled: ! isRated
                            text:
                                Logic.getPlayerString(gameModel.gameVariant, 2)
                        }
                    }
                }
            }
            RowLayout {
                Layout.topMargin: 0.6 * font.pixelSize
                Layout.fillWidth: true

                Label {
                    id: labelLevel

                    enabled: ! isRated
                    text: qsTr("Level %1").arg(slider.value)
                }
                Slider {
                    id: slider

                    enabled: ! isRated
                    from: 1; to: playerModel.maxLevel; stepSize: 1
                    // Implicit width of main contentItem might not fully fit
                    // on small screens if 2x2 check boxes are displayed. In
                    // this case, there is no need to clip the slider also,
                    // which expands to the dialog width if ! isDesktop
                    Layout.maximumWidth:
                        maxContentWidth - labelLevel.implicitWidth
                        - parent.spacing
                    Layout.fillWidth: true
                }
            }
        }
    }
}
