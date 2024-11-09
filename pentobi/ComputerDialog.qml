//-----------------------------------------------------------------------------
/** @file pentobi/ComputerDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "main.js" as Logic

PentobiDialog {
    id: root

    footer: DialogButtonBoxOkCancel { }
    onAboutToShow: {
        switch0.checked = computerPlays0
        switch1.checked = computerPlays1
        switch2.checked = computerPlays2
        switch3.checked = computerPlays3
        slider.value = playerModel.level
    }
    onAccepted: {
        computerPlays0 = switch0.checked
        computerPlays1 = switch1.checked
        computerPlays2 = switch2.checked
        computerPlays3 = switch3.checked
        if (! Logic.isComputerToPlay() || playerModel.level !== slider.value)
            Logic.cancelRunning()
        playerModel.level = slider.value
        if (! gameModel.isGameOver)
            Logic.checkComputerMove()
    }
    // Switch animations run even if the checked status is changed when the
    // switch is not visible. So we don't reuse the dialog.
    onClosed: computerDialog.source = ""

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
                Switch {
                    id: switch0

                    Layout.fillWidth: true
                    enabled: ! isRated
                    text: Logic.getPlayerString(gameModel.gameVariant, 0)
                    checked: computerPlays0
                    onClicked:
                        if (gameModel.nuColors === 4
                                && gameModel.nuPlayers === 2)
                            switch2.checked = checked
                }
                Switch {
                    id: switch1

                    enabled: ! isRated
                    text: Logic.getPlayerString(gameModel.gameVariant, 1)
                    checked: computerPlays1
                    onClicked:
                        if (gameModel.nuColors === 4
                                && gameModel.nuPlayers === 2)
                            switch3.checked = checked
                    Layout.fillWidth: true
                }
                Switch {
                    id: switch2

                    visible: gameModel.nuPlayers > 2
                    enabled: ! isRated
                    text: Logic.getPlayerString(gameModel.gameVariant, 2)
                    checked: computerPlays2
                    Layout.fillWidth: true
                }
                Switch {
                    id: switch3

                    visible: gameModel.nuPlayers > 3
                    enabled: ! isRated
                    text: Logic.getPlayerString(gameModel.gameVariant, 3)
                    checked: computerPlays3
                    Layout.fillWidth: true
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
                    snapMode: Slider.SnapOnRelease
                    Layout.fillWidth: true
                }
            }
        }
    }
}
