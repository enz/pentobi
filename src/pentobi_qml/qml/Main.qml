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

    minimumWidth: 240; minimumHeight: 252
    width: Screen.desktopAvailableWidth; height: Screen.desktopAvailableHeight
    visible: true
    color: theme.backgroundColor
    title: qsTr("Pentobi")
    onClosing: Qt.quit()
    Component.onCompleted: Logic.init()
    Component.onDestruction: Logic.quit()
    // Currently, we don't use the QtQuick ToolBar/MenuBar on Android. The file
    // dialog is unusable with dark themes (QTBUG-48324) and a white toolbar is
    // too distracting with the dark background we use on Android.
    menuBar: menuBarLoader.item
    toolBar: toolBarLoader.item

    ColumnLayout {
        anchors.fill: parent
        Keys.onReleased: if (isAndroid && event.key === Qt.Key_Menu) {
                             menuLoader.item.popup()
                             event.accepted = true
                         }

        Loader {
            sourceComponent: isAndroid ? androidToolBarComponent : undefined
            Layout.fillWidth: true

            Component {
                id: androidToolBarComponent

                AndroidToolBar {
                    id: toolBar

                    onPopupMenu: menuLoader.item.popup()
                }
            }
        }
        GameDisplay {
            id: gameDisplay

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
            }
        }
    }
    Loader {
        id: menuLoader

        sourceComponent: isAndroid ? menuComponent : undefined

        Component {
            id: menuComponent

            Menu {
                MenuGame { }
                MenuGo { }
                MenuEdit { }
                MenuComputer { }
                MenuView { }
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

        property alias folder: root.folder
        property alias markLastMove: gameDisplay.markLastMove
        property alias computerPlays0: root.computerPlays0
        property alias computerPlays1: root.computerPlays1
        property alias computerPlays2: root.computerPlays2
        property alias computerPlays3: root.computerPlays3
    }
    GameModel { id: gameModel }
    PlayerModel {
        id: playerModel

        onMoveGenerated: Logic.moveGenerated(move)
    }
    Loader { id: computerColorDialogLoader }
    Component {
        id: computerColorDialogComponent

        ComputerColorDialog {
            gameVariant: gameModel.gameVariant
            onAccepted: {
                root.computerPlays0 = this.computerPlays0
                root.computerPlays1 = this.computerPlays1
                root.computerPlays2 = this.computerPlays2
                root.computerPlays3 = this.computerPlays3
                Logic.cancelGenMove()
                Logic.checkComputerMove()
                gameDisplay.forceActiveFocus() // QTBUG-48456
            }
            onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456
        }
    }
    Loader { id: openDialogLoader }
    Component {
        id: openDialogComponent

        FileDialog {
            title: qsTr("Open")
            folder: root.folder == "" ? shortcuts.desktop : root.folder
            nameFilters:
                [ qsTr("Blokus games (*.blksgf)"), qsTr("All files (*)") ]
            onAccepted: {
                Logic.openFileUrl(fileUrl)
                root.folder = folder
                gameDisplay.forceActiveFocus() // QTBUG-48456
            }
            onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456
        }
    }
    Loader { id: saveDialogLoader }
    Component {
        id: saveDialogComponent

        FileDialog {
            title: qsTr("Save")
            selectExisting: false
            folder: root.folder == "" ? shortcuts.desktop : root.folder
            nameFilters:
                [ qsTr("Blokus games (*.blksgf)"), qsTr("All files (*)") ]
            onAccepted: {
                Logic.saveFileUrl(fileUrl)
                root.folder = folder
                gameDisplay.forceActiveFocus() // QTBUG-48456
            }
            onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456
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
    // Call a function that might block the GUI thread (e.g. initializing or
    // changing the game variant and the creation of the new pieces takes
    // several seconds on a ~1GHz ARM CPU). The call() function sets the
    // busy cursor to true and then calls the actual function with a small
    // delay to ensure that the running busy cursor is visible first.
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
                gameModel.autoSave()
    }
}
