import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "." as Pentobi

Pentobi.Dialog {
    property alias computerPlays0: checkBox0.checked
    property alias computerPlays1: checkBox1.checked
    property alias computerPlays2: checkBox2.checked
    property alias computerPlays3: checkBox3.checked
    property alias level: spinBox.value

    title: isDesktop ? qsTr("Computer Colors") : ""
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
                    columns: 2

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
                    }
                    CheckBox {
                        id: checkBox2

                        enabled: ! isRated
                        text: qsTr("Red")
                        visible: gameModel.nuPlayers > 2
                    }
                    CheckBox {
                        id: checkBox3

                        enabled: ! isRated
                        text: qsTr("Green")
                        visible: gameModel.nuPlayers > 3
                    }
                }
            }
            ColumnLayout {
                Label {
                    text:
                        switch (gameModel.gameVariant)
                        {
                        case "classic": return qsTr("Level (Classic, 4 players):")
                        case "classic_2": return qsTr("Level (Classic, 2 players):")
                        case "classic_3": return qsTr("Level (Classic, 3 players):")
                        case "duo": return qsTr("Level (Duo):")
                        case "junior": return qsTr("Level (Junior):")
                        case "trigon": return qsTr("Level (Trigon, 4 players):")
                        case "trigon_2": return qsTr("Level (Trigon, 2 players):")
                        case "trigon_3": return qsTr("Level (Trigon, 3 players):")
                        case "nexos": return qsTr("Level (Nexos, 4 players):")
                        case "nexos_2": return qsTr("Level (Nexos, 2 players):")
                        case "callisto": return qsTr("Level (Callisto, 4 players):")
                        case "callisto_2": return qsTr("Level (Callisto, 2 players, 2 colors):")
                        case "callisto_2_4": return qsTr("Level (Callisto, 2 players, 4 colors):")
                        case "callisto_3": return qsTr("Level (Callisto, 3 players):")
                        case "gembloq": return qsTr("Level (GembloQ, 4 players):")
                        case "gembloq_2": return qsTr("Level (GembloQ, 2 players, 2 colors):")
                        case "gembloq_2_4": return qsTr("Level (GembloQ, 2 players, 4 colors):")
                        case "gembloq_3": return qsTr("Level (GembloQ, 3 players):")
                        }
                }
                SpinBox {
                    id: spinBox

                    enabled: ! isRated
                    from: 1; to: playerModel.maxLevel; stepSize: 1
                }
            }
        }
    }
}
