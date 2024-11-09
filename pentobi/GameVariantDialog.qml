//-----------------------------------------------------------------------------
/** @file pentobi/GameVariantDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "main.js" as Logic

PentobiDialog {
    property string gameVariant

    footer: DialogButtonBoxOkCancel { }
    onAboutToShow: {
        gameVariant = gameModel.gameVariant
        if (gameVariant.startsWith("classic")) comboBox.currentIndex = 0
        else if (gameVariant === "duo") comboBox.currentIndex = 1
        else if (gameVariant === "junior") comboBox.currentIndex = 2
        else if (gameVariant.startsWith("trigon")) comboBox.currentIndex = 3
        else if (gameVariant.startsWith("nexos")) comboBox.currentIndex = 4
        else if (gameVariant.startsWith("gembloq")) comboBox.currentIndex = 5
        else if (gameVariant.startsWith("callisto")) comboBox.currentIndex = 6
    }
    onAccepted: Logic.changeGameVariant(gameVariant)

    Item {
        implicitWidth:
            Math.max(Math.min(columnLayout.implicitWidth, maxContentWidth),
                     minContentWidth)
        implicitHeight: columnLayout.implicitHeight


        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            ComboBox {
                id: comboBox

                model: [
                    qsTr("Classic"), qsTr("Duo"), qsTr("Junior"),
                    qsTr("Trigon"), qsTr("Nexos"), qsTr("GembloQ"),
                    qsTr("Callisto") ]
                onCurrentIndexChanged:
                    switch (currentIndex) {
                    case 0:
                        if (! gameVariant.startsWith("classic"))
                            gameVariant = "classic_2"
                        break
                    case 1:
                        if (gameVariant !== "duo")
                            gameVariant = "duo"
                        break
                    case 2:
                        if (gameVariant !== "junior")
                            gameVariant = "junior"
                        break
                    case 3:
                        if (! gameVariant.startsWith("trigon"))
                            gameVariant = "trigon_2"
                        break
                    case 4:
                        if (! gameVariant.startsWith("nexos"))
                            gameVariant = "nexos_2"
                        break
                    case 5:
                        if (! gameVariant.startsWith("gembloq"))
                            gameVariant = "gembloq_2"
                        break
                    case 6:
                        if (! gameVariant.startsWith("callisto"))
                            gameVariant = "callisto_2"
                        break
                    }
                Layout.fillWidth: true
            }
            GridLayout {
                columns: 2
                Layout.fillWidth: true

                Label {
                    text: qsTr("Players:")
                    Layout.fillWidth: true
                }
                RowLayout {
                    Layout.fillWidth: true

                    RadioButton {
                        text: "2"
                        checked: gameVariant === "classic_2"
                                 || gameVariant === "duo"
                                 || gameVariant === "junior"
                                 || gameVariant === "trigon_2"
                                 || gameVariant === "nexos_2"
                                 || gameVariant === "gembloq_2"
                                 || gameVariant === "callisto_2"
                                 || gameVariant === "gembloq_2_4"
                                 || gameVariant === "callisto_2_4"
                        onClicked:
                            if (checked) {
                                if (gameVariant.startsWith("classic"))
                                    gameVariant = "classic_2"
                                else if (gameVariant.startsWith("trigon"))
                                    gameVariant = "trigon_2"
                                else if (gameVariant.startsWith("nexos"))
                                    gameVariant = "nexos_2"
                                else if (gameVariant === "callisto")
                                    gameVariant = "callisto_2_4"
                                else if (gameVariant === "callisto_3")
                                    gameVariant = "callisto_2"
                                else if (gameVariant === "gembloq")
                                    gameVariant = "gembloq_2_4"
                                else if (gameVariant === "gembloq_3")
                                    gameVariant = "gembloq_2"
                            }
                        Layout.fillWidth: true
                    }
                    RadioButton {
                        text: "3"
                        opacity: enabled
                        enabled: gameVariant.startsWith("classic")
                                 || gameVariant.startsWith("trigon")
                                 || gameVariant.startsWith("gembloq")
                                 || gameVariant.startsWith("callisto")
                        checked: gameVariant === "classic_3"
                                 || gameVariant === "trigon_3"
                                 || gameVariant === "gembloq_3"
                                 || gameVariant === "callisto_3"
                        onClicked:
                            if (checked) {
                                if (gameVariant.startsWith("classic"))
                                    gameVariant = "classic_3"
                                else if (gameVariant.startsWith("trigon"))
                                    gameVariant = "trigon_3"
                                else if (gameVariant.startsWith("gembloq"))
                                    gameVariant = "gembloq_3"
                                else if (gameVariant.startsWith("callisto"))
                                    gameVariant = "callisto_3"
                            }
                        Layout.fillWidth: true
                    }
                    RadioButton {
                        text: "4"
                        opacity: enabled
                        enabled: gameVariant.startsWith("classic")
                                 || gameVariant.startsWith("trigon")
                                 || gameVariant.startsWith("nexos")
                                 || gameVariant.startsWith("gembloq")
                                 || gameVariant.startsWith("callisto")
                        checked: gameVariant === "classic"
                                 || gameVariant === "trigon"
                                 || gameVariant === "nexos"
                                 || gameVariant === "gembloq"
                                 || gameVariant === "callisto"
                        onClicked:
                            if (checked) {
                                if (gameVariant.startsWith("classic"))
                                    gameVariant = "classic"
                                else if (gameVariant.startsWith("trigon"))
                                    gameVariant = "trigon"
                                else if (gameVariant.startsWith("nexos"))
                                    gameVariant = "nexos"
                                else if (gameVariant.startsWith("gembloq"))
                                    gameVariant = "gembloq"
                                else if (gameVariant.startsWith("callisto"))
                                    gameVariant = "callisto"
                            }
                        Layout.fillWidth: true
                    }
                }
                Label {
                    text: qsTr("Colors:")
                    Layout.fillWidth: true
                }
                RowLayout {
                    Layout.fillWidth: true

                    RadioButton {
                        text: "2"
                        opacity: gameVariant === "duo"
                                 || gameVariant === "junior"
                                 || gameVariant === "gembloq_2"
                                 || gameVariant === "gembloq_2_4"
                                 || gameVariant === "callisto_2"
                                 || gameVariant === "callisto_2_4"
                        enabled: gameVariant === "duo"
                                 || gameVariant === "junior"
                                 || gameVariant.startsWith("gembloq")
                                 || gameVariant.startsWith("callisto")
                        checked: gameVariant === "duo"
                                 || gameVariant === "junior"
                                 || gameVariant === "gembloq_2"
                                 || gameVariant === "callisto_2"
                        onClicked:
                            if (checked) {
                                if (gameVariant.startsWith("callisto"))
                                    gameVariant = "callisto_2"
                                else if (gameVariant.startsWith("gembloq"))
                                    gameVariant = "gembloq_2"
                            }
                        Layout.fillWidth: true
                    }
                    RadioButton {
                        text: "3"
                        opacity: checked
                        enabled: gameVariant.startsWith("trigon")
                                 || gameVariant.startsWith("gembloq")
                                 || gameVariant.startsWith("callisto")
                        checked: gameVariant === "trigon_3"
                                 || gameVariant === "gembloq_3"
                                 || gameVariant === "callisto_3"
                        onClicked:
                            if (checked) {
                                if (gameVariant.startsWith("trigon"))
                                    gameVariant = "trigon_3"
                                else if (gameVariant.startsWith("gembloq"))
                                    gameVariant = "gembloq_3"
                                else if (gameVariant.startsWith("callisto"))
                                    gameVariant = "callisto_3"
                            }
                        Layout.fillWidth: true
                    }
                    RadioButton {
                        text: "4"
                        opacity: gameVariant.startsWith("classic")
                                 || gameVariant === "trigon"
                                 || gameVariant === "trigon_2"
                                 || gameVariant.startsWith("nexos")
                                 || gameVariant === "gembloq"
                                 || gameVariant === "gembloq_2"
                                 || gameVariant === "gembloq_2_4"
                                 || gameVariant === "callisto"
                                 || gameVariant === "callisto_2"
                                 || gameVariant === "callisto_2_4"
                        enabled: gameVariant.startsWith("classic")
                                 || gameVariant.startsWith("trigon")
                                 || gameVariant.startsWith("nexos")
                                 || gameVariant.startsWith("gembloq")
                                 || gameVariant.startsWith("callisto")
                        checked: gameVariant.startsWith("classic")
                                 || gameVariant === "trigon"
                                 || gameVariant === "trigon_2"
                                 || gameVariant.startsWith("nexos")
                                 || gameVariant === "gembloq"
                                 || gameVariant === "gembloq_2_4"
                                 || gameVariant === "callisto"
                                 || gameVariant === "callisto_2_4"
                        onClicked:
                            if (checked) {
                                if (gameVariant.startsWith("trigon"))
                                    gameVariant = "trigon"
                                else if (gameVariant.startsWith("nexos"))
                                    gameVariant = "nexos"
                                else if (gameVariant === "gembloq_2")
                                    gameVariant = "gembloq_2_4"
                                else if (gameVariant === "gembloq_3")
                                    gameVariant = "gembloq"
                                else if (gameVariant === "callisto_2")
                                    gameVariant = "callisto_2_4"
                                else if (gameVariant === "callisto_3")
                                    gameVariant = "callisto"
                            }
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}
