import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
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
    property bool isAndroid: Qt.platform.os === "android"
    property string themeName: isAndroid ? "dark" : "light"
    property QtObject theme: Logic.createTheme(themeName)
    property url folder
    property int defaultWidth:
        isAndroid ? Screen.desktopAvailableWidth :
                    Math.min(Screen.desktopAvailableWidth,
                             Math.round(Screen.pixelDensity / 3.5 * 600))
    property int defaultHeight:
        isAndroid ? Screen.desktopAvailableHeight :
                    Math.min(Screen.desktopAvailableHeight,
                             Math.round(Screen.pixelDensity / 3.5 * 800))

    function cancelGenMove() {
        playerModel.cancelGenMove()
        delayedCheckComputerMove.stop()
    }

    // Minimum size corresponds to a QVGA mobile device with 19px statusbar
    minimumWidth: 240; minimumHeight: 301

    width: defaultWidth; height: defaultHeight
    visibility: Window.AutomaticVisibility
    color: theme.backgroundColor
    title: qsTr("Pentobi")
    onClosing: Qt.quit()
    // Currently, we don't use the QtQuick ToolBar/MenuBar on Android. The file
    // dialog is unusable with dark themes (QTBUG-48324) and a white toolbar is
    // too distracting with the dark background we use on Android.
    menuBar: menuBarLoader.item
    toolBar: toolBarLoader.item
    Component.onCompleted: {
        Logic.init()
        show()
    }
    Component.onDestruction: gameModel.autoSave()

    ColumnLayout {
        anchors.fill: parent
        Keys.onReleased:
            if (isAndroid && event.key === Qt.Key_Menu) {
                androidToolBarLoader.item.popupMenu()
                event.accepted = true
            }
            else if (event.key === Qt.Key_Back && isAndroid && visibility === Window.FullScreen) {
                visibility = Window.AutomaticVisibility
                event.accepted = true
            }

        Loader {
            id: androidToolBarLoader

            sourceComponent: isAndroid ? androidToolBarComponent : undefined
            Layout.fillWidth: true

            Component {
                id: androidToolBarComponent

                AndroidToolBar {
                    visible: ! (visibility === Window.FullScreen && isAndroid)
                }
            }
        }
        GameDisplay {
            id: gameDisplay

            busyIndicatorRunning: pieces0 === undefined
                                  || lengthyCommand.isRunning
                                  || playerModel.isGenMoveRunning
            Layout.fillWidth: true
            Layout.fillHeight: true
            focus: true
            onPlay: Logic.play(pieceModel, gameCoord)
        }
    }
    Loader {
        id: menuBarLoader

        sourceComponent: isAndroid ? undefined : menuBarComponent

        Component {
            id: menuBarComponent

            MenuBar {
                MenuGame { }
                MenuGo { }
                MenuEdit { }
                MenuComputer { }
                MenuView { }
                MenuHelp { }
            }
        }
    }
    Loader {
        id: toolBarLoader

        sourceComponent: isAndroid ? undefined : toolBarComponent

        Component {
            id: toolBarComponent

            Pentobi.ToolBar { }
        }
    }
    Settings {
        id: settings

        property alias x: root.x
        property alias y: root.y
        property alias width: root.width
        property alias height: root.height
        property alias folder: root.folder
        property alias enableAnimations: gameDisplay.enableAnimations
        property alias markLastMove: gameDisplay.markLastMove
        property alias computerPlays0: root.computerPlays0
        property alias computerPlays1: root.computerPlays1
        property alias computerPlays2: root.computerPlays2
        property alias computerPlays3: root.computerPlays3
    }
    GameModel {
        id: gameModel

        onPositionAboutToChange: cancelGenMove()
    }
    PlayerModel {
        id: playerModel

        onMoveGenerated: Logic.moveGenerated(move)
    }
    Loader { id: computerColorDialogLoader }
    Component {
        id: computerColorDialogComponent

        ComputerColorDialog {
            id: computerColorDialog

            gameVariant: gameModel.gameVariant
            onAccepted: {
                root.computerPlays0 = computerColorDialog.computerPlays0
                root.computerPlays1 = computerColorDialog.computerPlays1
                root.computerPlays2 = computerColorDialog.computerPlays2
                root.computerPlays3 = computerColorDialog.computerPlays3
                if (! Logic.isComputerToPlay())
                    cancelGenMove()
                else if (! gameModel.isGameOver)
                    Logic.checkComputerMove()
                gameDisplay.forceActiveFocus() // QTBUG-48456
            }
            onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456
        }
    }
    Loader {
        id: openDialog

        function open() {
            if (status === Loader.Null)
                setSource("OpenDialog.qml")
            item.open()
        }
    }
    Loader {
        id: saveDialog

        function open() {
            if (status === Loader.Null)
                source = "SaveDialog.qml"
            item.open()
        }
    }
    Loader { id: errorMessageLoader }
    Component {
        id: errorMessageComponent

        MessageDialog {
            icon: StandardIcon.Critical
        }
    }
    Loader { id: infoMessageLoader }
    Component {
        id: infoMessageComponent

        MessageDialog { }
    }
    Loader { id: questionMessageLoader }
    Component {
        id: questionMessageComponent

        MessageDialog {
            standardButtons: StandardButton.Ok | StandardButton.Cancel
        }
    }
    // Used to delay calls to Logic.checkComputerMove such that the computer
    // starts thinking and the busy indicator is visible after the current move
    // placement animation has finished
    Timer {
        id: delayedCheckComputerMove

        interval: 400
        onTriggered: Logic.checkComputerMove()
    }
    // Delay lengthy function calls such that the busy indicator is visible
    Timer {
        id: lengthyCommand

        property bool isRunning
        property var func

        function run(func) {
            lengthyCommand.func = func
            isRunning = true
            restart()
        }

        interval: 400
        onTriggered: {
            func()
            isRunning = false
        }
    }
    Connections {
        target: Qt.application
        onStateChanged:
            if (Qt.application.state === Qt.ApplicationSuspended)
                gameModel.autoSave()
    }
}
