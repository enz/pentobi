//-----------------------------------------------------------------------------
/** @file pentobi_qml/qml/Main.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.0
import Qt.labs.settings 1.0
import QtMultimedia 5.0
import pentobi 1.0
import "." as Pentobi
import "Main.js" as Logic

ApplicationWindow {
    id: root

    property bool computerPlays0
    property bool computerPlays1
    property bool computerPlays2
    property bool computerPlays3
    property bool isMoveHintRunning
    property bool isSoundEnabled: true
    property string themeName: "dark"
    property QtObject theme: Logic.createTheme(themeName)

    // For a desktop window, we should use a smaller initial size and remember
    // the last size in the settings, but for now we use pentobi_qml only for
    // Android and on Android, initializing the window size with the available
    // screen size avoids flickering and delay due to multiple changes of the
    // window size at start-up (last tested with Qt 5.3.2).
    contentItem { minimumWidth: 240; minimumHeight: 300 }
    width: Screen.desktopAvailableWidth; height: Screen.desktopAvailableHeight

    visible: true
    title: qsTr("Pentobi")
    menuBar: Pentobi.Menu { }
    onClosing: Qt.quit()
    Component.onCompleted: {
        busyIndicator.running = false
        var autoSaveLoaded = boardModel.loadAutoSave()
        Logic.initGameVariant(boardModel.gameVariant)
        if (! autoSaveLoaded)
            Logic.initComputerColors()
        else if (boardModel.isGameOver)
            Logic.showGameOver()
        else
            Logic.checkComputerMove()
    }
    Component.onDestruction: Logic.quit()

    // Ensure sane values in case the values in the settings are unusable
    onThemeNameChanged: {
        switch (themeName) {
        case "light":
        case "dark":
            return
        default:
            console.log("fixing invalid theme name", themeName)
            themeName = "light"
        }
    }

    Settings {
        id: settings

        property alias themeName: root.themeName
        property alias isSoundEnabled: root.isSoundEnabled
        property alias computerPlays0: root.computerPlays0
        property alias computerPlays1: root.computerPlays1
        property alias computerPlays2: root.computerPlays2
        property alias computerPlays3: root.computerPlays3
    }

    BoardModel {
        id: boardModel
    }

    PlayerModel {
        id: playerModel

        onMoveGenerated: Logic.moveGenerated(move)
    }

    GameDisplay {
        id: gameDisplay

        focus: true
        anchors.fill: contentItem
        onPlay: Logic.play(pieceModel, gameCoord)
    }

    Loader { id: messageLoader }
    Component {
        id: messageComponent

        Pentobi.Message {
            id: message

            fontSize: 0.4 * gameDisplay.pieceAreaSize
            x: (root.width - width) / 2
            y: gameDisplay.y + gameDisplay.pieceSelectorY +
               (gameDisplay.pieceSelectorHeight - height) / 2
            onClicked: clear()
        }
    }

    BusyIndicator {
        id: busyIndicator

        running: true
        x: (root.width - width) / 2
        y: gameDisplay.y + gameDisplay.pieceSelectorY +
           (gameDisplay.pieceSelectorHeight - height) / 2
    }

    Loader { id: computerColorDialogLoader }
    Component {
        id: computerColorDialogComponent

        ComputerColorDialog {
            visible: false
            gameVariant: boardModel.gameVariant
            onAccepted: {
                visible = false
                root.computerPlays0 = this.computerPlays0
                root.computerPlays1 = this.computerPlays1
                root.computerPlays2 = this.computerPlays2
                root.computerPlays3 = this.computerPlays3
                Logic.cancelGenMove()
                Logic.checkComputerMove()
            }
            onRejected: visible = false
        }
    }

    // Used to delay calls to Logic.checkComputerMove such that the computer
    // starts thinking and the busy indicator is visible after the current move
    // placement animation has finished
    Timer {
        id: delayedCheckComputerMove

        interval: 450
        onTriggered: {
            Logic.playSound()
            Logic.checkComputerMove()
        }
    }
    // Call Logic.changeGameVariant with a small delay, such that the
    // running busy cursor is visible first
    Timer {
        id: changeGameVariantTimer

        property string _gameVariant

        function change(gameVariant) {
            _gameVariant = gameVariant
            busyIndicator.running = true
            start()
        }

        interval: 10
        onTriggered: {
            busyIndicator.running = false
            Logic.changeGameVariant(_gameVariant)
        }
    }

    Loader { id: soundEffectLoader }
    Component {
        id: soundEffectComponent

        SoundEffect { source: "sounds/play.wav"; volume: 0.7 }
    }
}
