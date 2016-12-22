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
    property bool isPlaySingleMoveRunning
    property bool isRated
    property int maxLevel: 7

    // Was computer thinking on regular game move when game was autosaved?
    // If yes, it will automatically start a move generation on startup.
    property bool wasGenMoveRunning

    property bool isAndroid: Qt.platform.os === "android"
    property bool useAndroidToolbar: isAndroid
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
    property int exportImageWidth: 400

    function cancelRunning() {
        analyzeGameModel.cancel()
        playerModel.cancelGenMove()
        delayedCheckComputerMove.stop()
    }

    minimumWidth: 240; minimumHeight: 320
    width: isAndroid ? Screen.desktopAvailableWidth : defaultWidth
    height: isAndroid ? Screen.desktopAvailableHeight : defaultHeight
    // On Android, initialize visible to true because startup time is slow and
    // we want to show the window as soon as possible. On the desktop, visible
    // is set to true in Component.onCompleted to reduce flickering.
    visible: isAndroid
    color: theme.backgroundColor
    title: Logic.getTitle(gameModel.file, gameModel.isModified)
    onClosing: Qt.quit()
    // Currently, we don't use the QtQuick ToolBar/MenuBar on Android. The file
    // dialog is unusable with dark themes (QTBUG-48324) and a white toolbar is
    // too distracting with the dark background we use on Android.
    menuBar: menuBarLoader.item
    toolBar: toolBarLoader.item
    statusBar: statusBarLoader.item
    Component.onCompleted: {
        Logic.init()
        show()
    }
    Component.onDestruction: Logic.autoSave()

    ColumnLayout {
        anchors.fill: parent
        Keys.onReleased: if (isAndroid && event.key === Qt.Key_Menu) {
                             androidToolBarLoader.item.popupMenu()
                             event.accepted = true
                         }

        Loader {
            id: androidToolBarLoader

            sourceComponent: useAndroidToolbar ? androidToolBarComponent : undefined
            Layout.fillWidth: true

            Component {
                id: androidToolBarComponent

                AndroidToolBar {
                    title: {
                        if (isRated) return qsTr("Rated game")
                        if (gameModel.file === "") return ""
                        return root.title
                    }
                }
            }
        }
        GameDisplay {
            id: gameDisplay

            busyIndicatorRunning: pieces0 === undefined
                                  || lengthyCommand.isRunning
                                  || playerModel.isGenMoveRunning
                                  || analyzeGameModel.isRunning
            Layout.fillWidth: true
            Layout.fillHeight: true
            focus: true
            onPlay: Logic.play(pieceModel, gameCoord)
        }
    }
    Loader {
        id: menuBarLoader

        sourceComponent: useAndroidToolbar ? undefined : menuBarComponent

        Component {
            id: menuBarComponent

            MenuBar {
                MenuGame { }
                MenuGo { }
                MenuEdit { }
                MenuView { }
                MenuComputer { }
                MenuTools { }
                MenuHelp { }
            }
        }
    }
    Loader {
        id: toolBarLoader

        sourceComponent: useAndroidToolbar ? undefined : toolBarComponent

        Component {
            id: toolBarComponent

            Pentobi.ToolBar { }
        }
    }
    Loader {
        id: statusBarLoader

        sourceComponent: isAndroid ? undefined : statusBarComponent

        Component {
            id: statusBarComponent

            Pentobi.StatusBar { }
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
        property alias moveMarking: gameDisplay.moveMarking
        property alias showCoordinates: gameDisplay.showCoordinates
        property alias flickableContentX: gameDisplay.flickableContentX
        property alias setupMode: gameDisplay.setupMode
        property alias computerPlays0: root.computerPlays0
        property alias computerPlays1: root.computerPlays1
        property alias computerPlays2: root.computerPlays2
        property alias computerPlays3: root.computerPlays3
        property alias isRated: root.isRated
        property alias wasGenMoveRunning: root.wasGenMoveRunning
        property alias exportImageWidth: root.exportImageWidth
        property alias showVariations: gameModel.showVariations
    }
    GameModel {
        id: gameModel

        onPositionAboutToChange: cancelRunning()
    }
    PlayerModel {
        id: playerModel

        onMoveGenerated: Logic.moveGenerated(move)
    }
    AnalyzeGameModel { id: analyzeGameModel }
    RatingModel {
        id: ratingModel

        gameVariant: gameModel.gameVariant
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
                    cancelRunning()
                else if (! gameModel.isGameOver)
                    Logic.checkComputerMove()
            }
            onVisibleChanged: if (! visible) gameDisplay.forceActiveFocus() // QTBUG-48456
        }
    }
    DialogLoader { id: gameInfoDialog; url: "GameInfoDialog.qml" }
    DialogLoader { id: initialRatingDialog; url: "InitialRatingDialog.qml" }
    DialogLoader { id: openDialog; url: "OpenDialog.qml" }
    DialogLoader { id: ratingDialog; url: "RatingDialog.qml" }
    DialogLoader { id: exportImageDialog; url: "ExportImageDialog.qml" }
    DialogLoader { id: imageSaveDialog; url: "ImageSaveDialog.qml" }
    DialogLoader { id: asciiArtSaveDialog; url: "AsciiArtSaveDialog.qml" }
    DialogLoader { id: gotoMoveDialog; url: "GotoMoveDialog.qml" }
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

        Pentobi.MessageDialog { }
    }
    Loader { id: questionMessageLoader }
    Component {
        id: questionMessageComponent

        Pentobi.MessageDialog {
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
                Logic.autoSave()
    }
}
