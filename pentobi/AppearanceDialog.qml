//-----------------------------------------------------------------------------
/** @file pentobi/AppearanceDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

PentobiDialog {
    id: root

    property int currentMoveMarkingIndex
    property int currentCommentIndex

    footer: DialogButtonBoxOkCancel { }
    onAboutToShow: {
        switchCoordinates.checked = gameView.showCoordinates
        switchShowVariations.checked = gameModel.showVariations
        switchColorblind.checked = theme.colorblind
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
        if (gameView.commentMode === "always")
            currentCommentIndex = 0
        else if (gameView.commentMode === "never")
            currentCommentIndex = 2
        else
            currentCommentIndex = 1
        comboBoxComment.currentIndex = currentCommentIndex
        if (isDesktop)
            switchMoveNumber.checked = gameView.showMoveNumber
    }
    onAccepted: {
        gameView.showCoordinates = switchCoordinates.checked
        gameModel.showVariations = switchShowVariations.checked
        theme.colorblind = switchColorblind.checked
        switch (comboBoxMoveMarking.currentIndex) {
        case 0: gameView.moveMarking = "last_dot"; break
        case 1: gameView.moveMarking = "last_number"; break
        case 2: gameView.moveMarking = "all_number"; break
        case 3: gameView.moveMarking = "none"; break
        }
        switch (comboBoxComment.currentIndex) {
        case 0: gameView.commentMode = "always"; break
        case 1: gameView.commentMode = "as_needed"; break
        case 2: gameView.commentMode = "never"; break
        }
        if (isDesktop)
            gameView.showMoveNumber = switchMoveNumber.checked
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

            Switch {
                id: switchCoordinates

                text: qsTr("Coordinates")
                checked: gameView.showCoordinates
            }
            Switch {
                id: switchShowVariations

                text: qsTr("Show variations")
                checked: gameModel.showVariations
            }
            Switch {
                id: switchMoveNumber

                visible: isDesktop
                //: Check box in appearance dialog whether to show the
                //: move number in the status bar.
                text: qsTr("Move number")
                checked: gameView.showMoveNumber
            }
            Switch {
                id: switchColorblind

                text: qsTr("Colorblind")
                checked: rootWindow.theme.colorblind
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
                text: qsTr("Show comment:")
                Layout.topMargin: 0.5 * font.pixelSize
            }
            ComboBox {
                id: comboBoxComment

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
