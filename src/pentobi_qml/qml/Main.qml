import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0
import Qt.labs.settings 1.0
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
    property string themeName: Qt.platform.os === "android" ? "dark" : "light"
    property bool markLastMove: true
    property QtObject theme: Logic.createTheme(themeName)

    property var _pieceMarked0
    property var _pieceMarked1
    property var _pieceMarked2
    property var _pieceMarked3

    // For a desktop window, we should use a smaller initial size and remember
    // the last size in the settings, but for now we use pentobi_qml only for
    // Android and on Android, initializing the window size with the available
    // screen size avoids flickering and delay due to multiple changes of the
    // window size at start-up (last tested with Qt 5.3.2).
    contentItem { minimumWidth: 240; minimumHeight: 252 }
    width: Screen.desktopAvailableWidth; height: Screen.desktopAvailableHeight

    visible: true
    color: theme.backgroundColor
    title: qsTr("Pentobi")
    menuBar: Pentobi.Menu { }
    toolBar: Pentobi.ToolBar { }
    onClosing: Qt.quit()
    Component.onCompleted: Logic.init()
    Component.onDestruction: Logic.quit()

    Settings {
        id: settings

        property alias markLastMove: root.markLastMove
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
        Component.onCompleted: if (initFailed()) Logic.playerInitFail()
    }

    GameDisplay {
        id: gameDisplay

        focus: true
        anchors.fill: contentItem
        onPlay: Logic.play(pieceModel, gameCoord)
    }

    Loader { id: computerColorDialogLoader }
    Component {
        id: computerColorDialogComponent

        ComputerColorDialog {
            gameVariant: boardModel.gameVariant
            onAccepted: {
                root.computerPlays0 = this.computerPlays0
                root.computerPlays1 = this.computerPlays1
                root.computerPlays2 = this.computerPlays2
                root.computerPlays3 = this.computerPlays3
                Logic.cancelGenMove()
                Logic.checkComputerMove()
            }
        }
    }

    Loader { id: messageDialogLoader }
    Component {
        id: messageDialogComponent

        MessageDialog {
            standardButtons: StandardButton.Ok | StandardButton.Cancel
        }
    }

    Loader { id: errorDialogLoader }
    Component {
        id: errorDialogComponent

        MessageDialog { }
    }

    // Used to delay calls to Logic.checkComputerMove such that the computer
    // starts thinking and the busy indicator is visible after the current move
    // placement animation has finished
    Timer {
        id: delayedCheckComputerMove

        interval: 400
        onTriggered: Logic.checkComputerMove()
    }

    // Call a function that might block the GUI thread (e.g. initializing or
    // changing the game variant and the creation of the new pieces takes
    // several seconds on a ~1GHz ARM CPU). The call() function sets the
    // busy cursor to true and then calls the actual function with a small
    // delay to ensure that the running busy cursor is visible first.
    // In the future, we could create the pieces with incubateObject() instead,
    // but this is currently buggy (see for example QTBUG-35587)
    Timer {
        id: callDelayTimer

        property var _func

        function call(func) {
            gameDisplay.busyIndicatorRunning = true
            _func = func
            start()
        }

        interval: 10
        onTriggered: {
            gameDisplay.busyIndicatorRunning = false
            _func()
        }
    }

    Connections {
        target: Qt.application
        onStateChanged:
            if (Qt.application.state === Qt.ApplicationSuspended)
                Logic.autosave()
    }
}
