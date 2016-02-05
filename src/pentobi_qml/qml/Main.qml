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
        isAndroid ? Screen.desktopAvailableWidth :
                    Math.min(Math.round(Screen.pixelDensity / 3.5 * 800))

    minimumWidth: 240; minimumHeight: 252
    width: isAndroid ? Screen.desktopAvailableWidth : defaultWidth
    height: isAndroid ? Screen.desktopAvailableHeight : defaultHeight
    // On Android, initialize visible to true because startup time is slow and
    // we want to show the window as soon as possible. On the desktop, visible
    // is set to true in Component.onCompleted to reduce flickering.
    visible: isAndroid
    color: theme.backgroundColor
    title: qsTr("Pentobi")
    onClosing: Qt.quit()
    // Currently, we don't use the QtQuick ToolBar/MenuBar on Android. The file
    // dialog is unusable with dark themes (QTBUG-48324) and a white toolbar is
    // too distracting with the dark background we use on Android.
    menuBar: menuBarLoader.item
    toolBar: toolBarLoader.item
    Component.onCompleted: {
        // Settings might contain unusable geometry
        if (x < 0 || x + width > Screen.desktopAvailableWidth
                || y < 0 || y + height > Screen.desktopAvailableHeight) {
            if (width > Screen.desktopAvailableWidth
                    || height > Screen.desktopAvailableHeight) {
                width = defaultWidth
                height = defaultHeight
            }
            x = (Screen.desktopAvailableWidth - width) / 2
            y = (Screen.desktopAvailableHeight - height) / 2
        }
        Logic.init()
        visible = true
    }
    Component.onDestruction: Logic.quit()

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
                MenuHelp { }
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
    Connections {
        target: Qt.application
        onStateChanged:
            if (Qt.application.state === Qt.ApplicationSuspended)
                gameModel.autoSave()
    }
}
