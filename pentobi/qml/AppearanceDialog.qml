//-----------------------------------------------------------------------------
/** @file pentobi/qml/AppearanceDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
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
                checkBoxCoordinates.checked !== gameView.showCoordinates
                || checkBoxShowVariations.checked !== gameModel.showVariations
                || checkBoxAnimatePieces.checked !== gameView.enableAnimations
                || checkBoxMoveNumber.checked !== gameView.showMoveNumber
                || comboBoxTheme.currentIndex !== currentThemeIndex
                || comboBoxMoveMarking.currentIndex !== currentMoveMarkingIndex
                || comboBoxComment.currentIndex !== currentCommentIndex
        }
        ButtonOk { }
    }
    property DialogButtonBox footerMobile: DialogButtonBoxOkCancel { }
    property int currentThemeIndex
    property int currentMoveMarkingIndex
    property int currentCommentIndex

    footer: isDesktop ? footerDesktop : footerMobile
    onOpened: {
        checkBoxCoordinates.checked = gameView.showCoordinates
        checkBoxShowVariations.checked = gameModel.showVariations
        checkBoxAnimatePieces.checked = gameView.enableAnimations
        if (themeName === "dark")
            currentThemeIndex = 1
        else if (themeName === "colorblind-light")
            currentThemeIndex = 2
        else if (themeName === "colorblind-dark")
            currentThemeIndex = 3
        else if (themeName === "system")
            currentThemeIndex = isAndroid ? 1 : 4
        else
            currentThemeIndex = 0
        comboBoxTheme.currentIndex = currentThemeIndex
        if (gameView.moveMarking === "last_dot")
            currentMoveMarkingIndex = 0
        else if (gameView.moveMarking === "last_number")
            currentMoveMarkingIndex = 1
        else if (gameView.moveMarking === "all_number")
            currentMoveMarkingIndex = 2
        else if (gameView.moveMarking === "none")
            currentMoveMarkingIndex = 3
        else
            currentMoveMarkingIndex = 0
        comboBoxMoveMarking.currentIndex = currentMoveMarkingIndex
        if (isDesktop) {
            checkBoxMoveNumber.checked = gameView.showMoveNumber
            if (gameView.commentMode === "always")
                currentCommentIndex = 0
            else if (gameView.commentMode === "never")
                currentCommentIndex = 2
            else
                currentCommentIndex = 1
            comboBoxComment.currentIndex = currentCommentIndex
        }
    }
    onAccepted: {
        gameView.showCoordinates = checkBoxCoordinates.checked
        gameModel.showVariations = checkBoxShowVariations.checked
        gameView.enableAnimations = checkBoxAnimatePieces.checked
        switch (comboBoxTheme.currentIndex) {
        case 0: themeName = "light"; break
        case 1: themeName = "dark"; break
        case 2: themeName = "colorblind-light"; break
        case 3: themeName = "colorblind-dark"; break
        case 4: themeName = "system"; break
        }
        switch (comboBoxMoveMarking.currentIndex) {
        case 0: gameView.moveMarking = "last_dot"; break
        case 1: gameView.moveMarking = "last_number"; break
        case 2: gameView.moveMarking = "all_number"; break
        case 3: gameView.moveMarking = "none"; break
        }
        if (isDesktop) {
            gameView.showMoveNumber = checkBoxMoveNumber.checked
            switch (comboBoxComment.currentIndex) {
            case 0: gameView.commentMode = "always"; break
            case 1: gameView.commentMode = "as_needed"; break
            case 2: gameView.commentMode = "never"; break
            }
        }
    }
    onApplied: {
        onAccepted()
        onOpened()
    }

    Flickable {
        implicitWidth:
            Math.max(Math.min(columnLayout.implicitWidth, maxContentWidth),
                     minContentWidth)
        implicitHeight: Math.min(columnLayout.implicitHeight, maxContentHeight)
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
                id: checkBoxMoveNumber

                visible: isDesktop
                //: Check box in appearance dialog whether to show the
                //: move number in the status bar.
                text: qsTr("Move number")
            }
            CheckBox {
                id: checkBoxAnimatePieces

                text: qsTr("Animations")
            }
            Label {
                text: qsTr("Color theme:")
                Layout.topMargin: 0.5 * font.pixelSize


            }
            ComboBox {
                id: comboBoxTheme

                model: isAndroid ?
                           [
                               qsTr("Light"),
                               qsTr("Dark"),
                               qsTr("Colorblind light"),
                               qsTr("Colorblind dark")
                           ] :
                           [
                               qsTr("Light"),
                               qsTr("Dark"),
                               qsTr("Colorblind light"),
                               qsTr("Colorblind dark"),
                               //: Name of theme using default system colors
                               qsTr("System")
                           ]
                Layout.preferredWidth: font.pixelSize * 20
                Layout.fillWidth: true
            }
            Label {
                text: qsTr("Move marking:")
                Layout.topMargin: 0.5 * font.pixelSize


            }
            ComboBox {
                id: comboBoxMoveMarking

                model: [
                    qsTr("Last with dot"),
                    qsTr("Last with number"),
                    qsTr("All with number"),
                    //: Move marking/None
                    qsTr("None")
                ]
                Layout.preferredWidth: font.pixelSize * 20
                Layout.fillWidth: true
            }
            Label {
                visible: isDesktop
                text: qsTr("Show comment:")
                Layout.topMargin: 0.5 * font.pixelSize


            }
            ComboBox {
                id: comboBoxComment

                visible: isDesktop
                model: [
                    //: Show-comment mode
                    qsTr("Always"),
                    //: Show-comment mode
                    qsTr("As needed"),
                    //: Show-comment mode
                    qsTr("Never")
                ]
                Layout.preferredWidth: font.pixelSize * 20
                Layout.fillWidth: true
            }
        }
    }
}
