import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import Qt.labs.settings 1.0
import pentobi 1.0
import "." as Pentobi
import "Main.js" as Logic

Window {
    id: root

    property bool computerPlays0
    property bool computerPlays1: true
    property bool computerPlays2: true
    property bool computerPlays3: true
    property bool isPlaySingleMoveRunning
    property bool isRated
    property int maxLevel: 7

    // Was computer thinking on regular game move when game was autosaved?
    // If yes, it will automatically start a move generation on startup.
    property bool wasGenMoveRunning

    // If the user manually disabled all computer colors in the dialog, we
    // assume that they want to edit games rather than play, and we will not
    // initialize the computer colors on New Game but only clear the board.
    property bool initComputerColorsOnNewGame: true

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
    property int exportImageWidth: 400

    // Minimum size corresponds to a QVGA mobile device with 19px statusbar
    minimumWidth: 240; minimumHeight: 301

    width: defaultWidth; height: defaultHeight
    visibility: Window.AutomaticVisibility
    color: theme.backgroundColor
    title: qsTr("Pentobi")
    onClosing: Qt.quit()
    Component.onCompleted: {
        Logic.init()
        show()
    }
    Component.onDestruction: Logic.autoSave()

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Keys.onReleased:
            if (isAndroid && event.key === Qt.Key_Menu) {
                androidToolBarLoader.item.popupMenu()
                event.accepted = true
            }
            else if (event.key === Qt.Key_Back && isAndroid && visibility === Window.FullScreen) {
                visibility = Window.AutomaticVisibility
                event.accepted = true
            }

        Pentobi.ToolBar {
            visible: ! (visibility === Window.FullScreen && isAndroid)
            title: {
                if (isRated) return qsTr("Rated game")
                if (gameModel.file === "") return ""
                return Logic.getFileLabel(gameModel.file, gameModel.isModified)
            }
        }
        GameDisplay {
            id: gameDisplay

            theme: root.theme
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
        property alias swipeViewCurrentIndex: gameDisplay.swipeViewCurrentIndex
        property alias setupMode: gameDisplay.setupMode
        property alias computerPlays0: root.computerPlays0
        property alias computerPlays1: root.computerPlays1
        property alias computerPlays2: root.computerPlays2
        property alias computerPlays3: root.computerPlays3
        property alias initComputerColorsOnNewGame: root.initComputerColorsOnNewGame;
        property alias isRated: root.isRated
        property alias themeName: root.themeName
        property alias wasGenMoveRunning: root.wasGenMoveRunning
        property alias exportImageWidth: root.exportImageWidth
        property alias showVariations: gameModel.showVariations
    }
    GameModel {
        id: gameModel

        onPositionAboutToChange: Logic.cancelRunning()
    }
    PlayerModel {
        id: playerModel

        onMoveGenerated: Logic.moveGenerated(move)
    }
    AnalyzeGameModel { id: analyzeGameModel }
    RatingModel {
        id: ratingModel

        gameVariant: gameModel.gameVariant
        // There is a bug in the combination of ScrollView, GridLayout and
        // Repeater, which makes the rating dialog not reusable if the history
        // changes or the dialog is closed and opened again because empty rows
        // will appear in the GridLayout (last tested with Qt 5.8-rc, some bugs
        // only happen on Android, some also on Linux). As a workaround, we
        // force it to be recreated if the history changed or it is closed.
        onHistoryChanged: if (ratingDialog.item) ratingDialog.item.close()
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
                initComputerColorsOnNewGame = Logic.computerPlaysAny()
                if (! Logic.isComputerToPlay())
                    Logic.cancelRunning()
                else if (! gameModel.isGameOver)
                    Logic.checkComputerMove()
            }
            onVisibleChanged: if (! visible) gameDisplay.forceActiveFocus() // QTBUG-48456
        }
    }
    DialogLoader { id: gameInfoDialog; url: "GameInfoDialog.qml" }
    DialogLoader { id: initialRatingDialog; url: "InitialRatingDialog.qml" }
    DialogLoader { id: openDialog; url: "OpenDialog.qml" }
    DialogLoader { id: exportImageDialog; url: "ExportImageDialog.qml" }
    DialogLoader { id: imageSaveDialog; url: "ImageSaveDialog.qml" }
    DialogLoader { id: asciiArtSaveDialog; url: "AsciiArtSaveDialog.qml" }
    DialogLoader { id: gotoMoveDialog; url: "GotoMoveDialog.qml" }
    DialogLoader { id: ratingDialog; url: "RatingDialog.qml" }
    DialogLoader { id: saveDialog; url: "SaveDialog.qml" }
    Loader { id: helpWindowLoader }
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

    // Runs the openRatedGame() callback from RatingDialog in a new event
    // because RatingDialog destroys itself before the callback and otherwise
    // the environment for the callback is undefined (at least if another
    // verify abort game dialog is shown first).
    Timer {
        id: queuedOpenRatedGame

        property var byteArray

        interval: 0
        onTriggered: Logic.openRatedGame(byteArray)
    }

    Connections {
        target: Qt.application
        onStateChanged:
            if (Qt.application.state === Qt.ApplicationSuspended)
                Logic.autoSave()
    }
}
