//-----------------------------------------------------------------------------
/** @file pentobi/ScoreDisplay.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property int altPlayer: gameModel.altPlayer
    property real points0: gameModel.points0
    property real points1: gameModel.points1
    property real points2: gameModel.points2
    property real points3: gameModel.points3
    property real bonus0: gameModel.bonus0
    property real bonus1: gameModel.bonus1
    property real bonus2: gameModel.bonus2
    property real bonus3: gameModel.bonus3
    property bool hasMoves0: gameModel.hasMoves0
    property bool hasMoves1: gameModel.hasMoves1
    property bool hasMoves2: gameModel.hasMoves2
    property bool hasMoves3: gameModel.hasMoves3

    RowLayout {
        id: rowLayout

        width: root.width
        height: Math.min(root.height, 0.047 * root.width)
        anchors.centerIn: parent
        spacing: 0

        Item { Layout.fillWidth: true }
        ScoreElement2 {
            id: playerScore0

            visible: gameModel.nuColors === 4 && gameModel.nuPlayers === 2
            value: root.points0 + root.points2
            isFinal: ! root.hasMoves0 && ! root.hasMoves2
            fontSize: rowLayout.height
            color1: gameView.color0[0]
            color2: gameView.color2[0]
            // Avoid position changes unless score text gets really long
            Layout.minimumWidth: 3 * fontSize
        }
        Item { visible: playerScore0.visible; Layout.fillWidth: true }
        ScoreElement2 {
            id: playerScore1

            visible: gameModel.nuColors === 4 && gameModel.nuPlayers === 2
            value: root.points1 + root.points3
            isFinal: ! root.hasMoves1 && ! root.hasMoves3
            fontSize: rowLayout.height
            color1: gameView.color1[0]
            color2: gameView.color3[0]
            // Avoid position changes unless score text gets really long
            Layout.minimumWidth: 3 * fontSize
        }
        Item { visible: playerScore1.visible; Layout.fillWidth: true }
        ScoreElement {
            id: colorScore0

            value: root.points0
            bonus: root.bonus0
            isFinal: ! root.hasMoves0
            fontSize: rowLayout.height
            color: color0[0]
            // Avoid position changes unless score text gets really long
            Layout.minimumWidth: 2.3 * fontSize
        }
        Item { visible: colorScore0.visible; Layout.fillWidth: true }
        ScoreElement {
            id: colorScore1

            value: root.points1
            bonus: root.bonus1
            isFinal: ! root.hasMoves1
            fontSize: rowLayout.height
            color: color1[0]
            // Avoid position changes unless score text gets really long
            Layout.minimumWidth: 2.3 * fontSize
        }
        Item { visible: colorScore1.visible; Layout.fillWidth: true }
        ScoreElement {
            id: colorScore2

            visible: gameModel.nuColors > 2
            value: root.points2
            bonus: root.bonus2
            isFinal: ! root.hasMoves2
            fontSize: rowLayout.height
            color: color2[0]
            // Avoid position changes unless score text gets really long
            Layout.minimumWidth: 2.3 * fontSize
        }
        Item { visible: colorScore2.visible; Layout.fillWidth: true }
        ScoreElement {
            id: colorScore3

            visible: gameModel.nuColors > 3
                     && gameModel.gameVariant !== "classic_3"
            value: root.points3
            bonus: root.bonus3
            isFinal: ! root.hasMoves3
            fontSize: rowLayout.height
            color: color3[0]
            // Avoid position changes unless score text gets really long
            Layout.minimumWidth: 2.3 * fontSize
        }
        Item { visible: colorScore3.visible; Layout.fillWidth: true }
        ScoreElement2 {
            id: altColorIndicator

            visible: gameModel.gameVariant === "classic_3" && root.hasMoves3
            value: root.points3
            isAltColor: true
            isFinal: ! root.hasMoves3
            fontSize: rowLayout.height
            color1: theme.colorGreen[0]
            color2:
                switch (root.altPlayer) {
                case 0: return gameView.color0[0]
                case 1: return gameView.color1[0]
                case 2: return gameView.color2[0]
                }
        }
        Item { visible: altColorIndicator.visible; Layout.fillWidth: true }
    }
}
