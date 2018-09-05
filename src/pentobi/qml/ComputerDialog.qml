//-----------------------------------------------------------------------------
/** @file pentobi/qml/ComputerDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Dialog {
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
                     minContentWidth)
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            ColumnLayout {
                Layout.fillWidth: true

                Label { text: qsTr("Computer plays:") }
                GridLayout {
                    columns: gameModel.nuPlayers <= 2 ? 1 : 2

                    Row {
                        Rectangle {
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameDisplay.color0[0]
                        }
                        Rectangle {
                            visible: gameModel.nuColors === 4
                                     && gameModel.nuPlayers === 2
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameDisplay.color2[0]
                        }
                        CheckBox {
                            id: checkBox0

                            enabled: ! isRated
                            text: {
                                if (gameModel.nuColors === 4
                                        && gameModel.nuPlayers === 2)
                                    return qsTr("Blue/Red")
                                if (gameModel.gameVariant === "duo")
                                    return qsTr("Purple")
                                if (gameModel.gameVariant === "junior")
                                    return qsTr("Green")
                                return qsTr("Blue")
                            }
                            onClicked:
                                if (gameModel.nuColors === 4
                                        && gameModel.nuPlayers === 2)
                                    checkBox2.checked = checked
                        }
                    }
                    Row {
                        Rectangle {
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameDisplay.color1[0]
                        }
                        Rectangle {
                            visible: gameModel.nuColors === 4
                                     && gameModel.nuPlayers === 2
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameDisplay.color3[0]
                        }
                        CheckBox {
                            id: checkBox1

                            enabled: ! isRated
                            text: {
                                if (gameModel.nuColors === 4
                                        && gameModel.nuPlayers === 2)
                                    return qsTr("Yellow/Green")
                                if (gameModel.gameVariant === "duo"
                                        || gameModel.gameVariant === "junior")
                                    return qsTr("Orange")
                                if (gameModel.nuColors === 2)
                                    return qsTr("Green")
                                return qsTr("Yellow")
                            }
                            onClicked:
                                if (gameModel.nuColors === 4
                                        && gameModel.nuPlayers === 2)
                                    checkBox3.checked = checked
                        }
                    }
                    Row {
                        visible: gameModel.nuPlayers > 3

                        Rectangle {
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameDisplay.color3[0]
                        }
                        CheckBox {
                            id: checkBox3

                            enabled: ! isRated
                            text: qsTr("Green")
                        }
                    }
                    Row {
                        visible: gameModel.nuPlayers > 2

                        Rectangle {
                            width: font.pixelSize; height: width
                            radius: width / 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: gameDisplay.color2[0]
                        }
                        CheckBox {
                            id: checkBox2

                            enabled: ! isRated
                            text: qsTr("Red")
                        }
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true

                Label {
                    enabled: ! isRated
                    text: qsTr("Level %1").arg(slider.value)
                }
                Slider {
                    id: slider

                    enabled: ! isRated
                    from: 1; to: playerModel.maxLevel; stepSize: 1
                    Layout.fillWidth: true
                }
            }
        }
    }
}
