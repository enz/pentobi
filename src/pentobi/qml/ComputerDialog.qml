import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "." as Pentobi

Pentobi.Dialog {
    id: root

    property alias computerPlays0: checkBox0.checked
    property alias computerPlays1: checkBox1.checked
    property alias computerPlays2: checkBox2.checked
    property alias computerPlays3: checkBox3.checked
    property alias level: spinBox.value

    title: isDesktop ? qsTr("Computer Settings") : ""
    footer: OkCancelButtons { }

    Item {
        implicitWidth: {
            var w = columnLayout.implicitWidth
            // Avoid too small width because Default style in Qt 5.11 makes
            // footer no wider than content, which can cause elided text on
            // dialog buttons
            w = Math.max(w, font.pixelSize * 18)
            w = Math.min(w, 0.85 * rootWindow.width)
            return w
        }
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            ColumnLayout {
                Label { text: qsTr("Computer plays:") }
                GridLayout {
                    columns: 2; rows: 2

                    CheckBox {
                        id: checkBox0

                        enabled: ! isRated
                        text: {
                            if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                                return qsTr("Blue/Red")
                            if (gameModel.gameVariant === "duo")
                                return qsTr("Purple")
                            if (gameModel.gameVariant === "junior")
                                return qsTr("Green")
                            return qsTr("Blue")
                        }
                        onClicked:
                            if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                                computerPlays2 = checked
                        Layout.column: 0
                        Layout.row: 0
                    }
                    CheckBox {
                        id: checkBox1

                        enabled: ! isRated
                        text: {
                            if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                                return qsTr("Yellow/Green")
                            if (gameModel.gameVariant === "duo"
                                    || gameModel.gameVariant === "junior")
                                return qsTr("Orange")
                            if (gameModel.nuColors === 2)
                                return qsTr("Green")
                            return qsTr("Yellow")
                        }
                        onClicked:
                            if (gameModel.nuColors === 4 && gameModel.nuPlayers === 2)
                                computerPlays3 = checked
                        Layout.column: gameModel.nuPlayers === 2 ? 0 : 1
                        Layout.row: gameModel.nuPlayers === 2 ? 1 : 0
                    }
                    CheckBox {
                        id: checkBox2

                        enabled: ! isRated
                        text: qsTr("Red")
                        visible: gameModel.nuPlayers > 2
                        Layout.column: gameModel.nuColors === 3 ? 0 : 1
                        Layout.row: gameModel.nuColors === 3 ? 1 : 1
                        Layout.rowSpan: gameModel.nuColors === 3 ? 2 : 1
                    }
                    CheckBox {
                        id: checkBox3

                        enabled: ! isRated
                        text: qsTr("Green")
                        visible: gameModel.nuPlayers > 3
                        Layout.column: 0
                        Layout.row: 1
                    }
                }
            }
            Item { Layout.preferredWidth: 0.5 * root.font.pixelSize }
            RowLayout {
                Label { text: qsTr("Level:") }
                SpinBox {
                    id: spinBox

                    enabled: ! isRated
                    from: 1; to: playerModel.maxLevel; stepSize: 1
                }
            }
        }
    }
}
