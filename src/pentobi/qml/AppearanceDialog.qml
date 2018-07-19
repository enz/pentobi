import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "." as Pentobi

Pentobi.Dialog {
    id: root

    title: isDesktop ? qsTr("Appearance") : ""
    footer: OkCancelButtons { }
    onOpened: {
        checkBoxCoordinates.checked = gameDisplay.showCoordinates
        checkBoxShowVariations.checked = gameModel.showVariations
        checkBoxAnimatePieces.checked = gameDisplay.enableAnimations
        if (themeName === "light")
            comboBoxTheme.currentIndex = 0
        else if (themeName === "dark")
            comboBoxTheme.currentIndex = 1
        else if (themeName === "colorblind-light")
            comboBoxTheme.currentIndex = 2
        else if (themeName === "colorblind-dark")
            comboBoxTheme.currentIndex = 3
        if (gameDisplay.moveMarking === "last_dot")
            comboBoxMoveMarking.currentIndex = 0
        else if (gameDisplay.moveMarking === "last_number")
            comboBoxMoveMarking.currentIndex = 1
        else if (gameDisplay.moveMarking === "all_number")
            comboBoxMoveMarking.currentIndex = 2
        else if (gameDisplay.moveMarking === "none")
            comboBoxMoveMarking.currentIndex = 3

    }
    onAccepted: {
        gameDisplay.showCoordinates = checkBoxCoordinates.checked
        gameModel.showVariations = checkBoxShowVariations.checked
        gameDisplay.enableAnimations = checkBoxAnimatePieces.checked
        switch (comboBoxTheme.currentIndex) {
        case 0: themeName = "light"; break
        case 1: themeName = "dark"; break
        case 2: themeName = "colorblind-light"; break
        case 3: themeName = "colorblind-light"; break
        }
        switch (comboBoxMoveMarking.currentIndex) {
        case 0: gameDisplay.moveMarking = "last_dot"; break
        case 1: gameDisplay.moveMarking = "last_number"; break
        case 2: gameDisplay.moveMarking = "all_number"; break
        case 3: gameDisplay.moveMarking = "none"; break
        }
    }

    Flickable {
        implicitWidth: Math.min(font.pixelSize * 18, 0.9 * rootWindow.width)
        implicitHeight: Math.min(columnLayout.implicitHeight, 0.7 * rootWindow.height)
        contentHeight: columnLayout.implicitHeight
        clip: true

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            CheckBox {
                id: checkBoxCoordinates

                text: qsTr("Coordinates")
            }
            CheckBox {
                id: checkBoxShowVariations

                text: qsTr("Show variations")
            }
            CheckBox {
                id: checkBoxAnimatePieces

                text: qsTr("Animate pieces")
            }
            Label { text: qsTr("Theme:") }
            ComboBox {
                id: comboBoxTheme

                model: [
                    qsTr("Light"),
                    qsTr("Dark"),
                    qsTr("Colorblind light"),
                    qsTr("Colorblind dark")
                ]
                Layout.preferredWidth: font.pixelSize * 15

            }
            Label { text: qsTr("Move marking:") }
            ComboBox {
                id: comboBoxMoveMarking

                model: [
                    qsTr("Last with dot"),
                    qsTr("Last with number"),
                    qsTr("All with number"),
                    //: Move marking/None
                    qsTr("None")
                ]
                Layout.preferredWidth: font.pixelSize * 15

            }
        }
    }
}
