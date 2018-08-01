//-----------------------------------------------------------------------------
/** @file pentobi/qml/AppearanceDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.2
import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "." as Pentobi

Pentobi.Dialog {
    id: root

    // Mobile layout may not have enough screen space for apply button and the
    // immovable dialog will cover most of the screen anyway. Note that using
    // the same ButtonBox for both and setting visible to false for ButtonApply
    // if not desktop causes a binding loop for implicitWidth in Qt 5.11 and
    // elided Text on the dialog buttons.
    property DialogButtonBox footerDesktop: Pentobi.DialogButtonBox {
        ButtonCancel { }
        ButtonApply {
            enabled:
                checkBoxCoordinates.checked !== gameDisplay.showCoordinates
                || checkBoxShowVariations.checked !== gameModel.showVariations
                || checkBoxAnimatePieces.checked !== gameDisplay.enableAnimations
                || comboBoxTheme.currentIndex !== currentThemeIndex
                || comboBoxMoveMarking.currentIndex !== currentMoveMarkingIndex
        }
        ButtonOk { }
    }
    property DialogButtonBox footerMobile: DialogButtonBoxOkCancel { }
    property int currentThemeIndex
    property int currentMoveMarkingIndex

    footer: isDesktop ? footerDesktop : footerMobile
    onOpened: {
        checkBoxCoordinates.checked = gameDisplay.showCoordinates
        checkBoxShowVariations.checked = gameModel.showVariations
        checkBoxAnimatePieces.checked = gameDisplay.enableAnimations
        if (themeName === "dark")
            currentThemeIndex = 1
        else if (themeName === "colorblind-light")
            currentThemeIndex = 2
        else if (themeName === "colorblind-dark")
            currentThemeIndex = 3
        else if (themeName === "system")
            currentThemeIndex = 4
        else
            currentThemeIndex = 0
        comboBoxTheme.currentIndex = currentThemeIndex
        if (gameDisplay.moveMarking === "last_dot")
            currentMoveMarkingIndex = 0
        else if (gameDisplay.moveMarking === "last_number")
            currentMoveMarkingIndex = 1
        else if (gameDisplay.moveMarking === "all_number")
            currentMoveMarkingIndex = 2
        else if (gameDisplay.moveMarking === "none")
            currentMoveMarkingIndex = 3
        else
            currentMoveMarkingIndex = 0
        comboBoxMoveMarking.currentIndex = currentMoveMarkingIndex
    }
    onAccepted: {
        gameDisplay.showCoordinates = checkBoxCoordinates.checked
        gameModel.showVariations = checkBoxShowVariations.checked
        gameDisplay.enableAnimations = checkBoxAnimatePieces.checked
        switch (comboBoxTheme.currentIndex) {
        case 0: themeName = "light"; break
        case 1: themeName = "dark"; break
        case 2: themeName = "colorblind-light"; break
        case 3: themeName = "colorblind-dark"; break
        case 4: themeName = "system"; break
        }
        switch (comboBoxMoveMarking.currentIndex) {
        case 0: gameDisplay.moveMarking = "last_dot"; break
        case 1: gameDisplay.moveMarking = "last_number"; break
        case 2: gameDisplay.moveMarking = "all_number"; break
        case 3: gameDisplay.moveMarking = "none"; break
        }
    }
    onApplied: {
        onAccepted()
        onOpened()
    }

    Flickable {
        implicitWidth: Math.min(columnLayout.implicitWidth, maxContentWidth)
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
                    qsTr("Colorblind dark"),
                    //: Name of theme using default system colors
                    qsTr("System"),
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
