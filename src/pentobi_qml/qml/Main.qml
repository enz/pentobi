import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import Qt.labs.settings 1.0
import pentobi 1.0
import "." as Pentobi
import "Main.js" as Logic

ApplicationWindow {
    id: rootWindow

    property bool computerPlays0
    property bool computerPlays1: true
    property bool computerPlays2: true
    property bool computerPlays3: true
    property bool isPlaySingleMoveRunning
    property bool isRated

    // Assume larger screen with landscape orientation and mouse input.
    property bool isDesktop: ! isAndroid

    property int maxLevel: 7
    property alias gameDisplay: gameDisplayLoader.item

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
    property url folder: gameModel.getDefaultFolder()
    property int defaultWidth:
        isAndroid ? Screen.desktopAvailableWidth :
                    Math.min(Screen.desktopAvailableWidth, 1200)
    property int defaultHeight:
        isAndroid ? Screen.desktopAvailableHeight :
                    Math.min(Screen.desktopAvailableHeight, 680)
    property int exportImageWidth: 400
    property bool busyIndicatorRunning: gameDisplay.pieces0 === undefined
                                        || lengthyCommand.isRunning
                                        || playerModel.isGenMoveRunning
                                        || analyzeGameModel.isRunning

    property Actions actions: Actions { }

    minimumWidth: isDesktop ? 560 : 240
    minimumHeight: isDesktop ? 315 : 301
    width: defaultWidth; height: defaultHeight
    color: theme.backgroundColor
    //: Main window title if no file is loaded.
    title: qsTr("Pentobi")
    Component.onCompleted: {
        Logic.init()
        show()
    }
    Component.onDestruction: Logic.autoSave()

    ColumnLayout {
        property bool altKeyPressed

        anchors.fill: parent
        spacing: 0
        Keys.onPressed: altKeyPressed = (event.key === Qt.Key_Alt)
        Keys.onReleased:
            if (event.key === Qt.Key_Alt) {
                // We want the Alt key open the menu but not if it is used in a
                // combination used by the window manager (e.g. Alt-Tab)
                if (altKeyPressed)  {
                    toolBar.clickMenuButton()
                    event.accepted = true
                    altKeyPressed = false
                }
            }
            else if (event.key === Qt.Key_Back) {
                // Note that we cannot use an action with shortcut Back because
                // as of Qt 5.11.1 the action would always receive the Back key
                // even if a Controls2.Dialog is open that could be closed with
                // the back key if the Action did not exist.
                if (visibility === Window.FullScreen) {
                    visibility = Window.AutomaticVisibility
                    event.accepted = true
                }
        }

        Pentobi.ToolBar {
            id: toolBar

            visible: ! (visibility === Window.FullScreen && isAndroid)
        }
        Loader {
            id: gameDisplayLoader

            Layout.fillWidth: true
            Layout.fillHeight: true
            focus: true
            sourceComponent: isDesktop ? componentGameDisplayDesktop : componentGameDisplayMobile

            Component {
                id: componentGameDisplayDesktop

                GameDisplayDesktop {
                    theme: rootWindow.theme
                    busyIndicatorRunning: rootWindow.busyIndicatorRunning
                    onPlay: Logic.play(pieceModel, gameCoord)
                }
            }
            Component {
                id: componentGameDisplayMobile

                GameDisplayMobile {
                    theme: rootWindow.theme
                    busyIndicatorRunning: rootWindow.busyIndicatorRunning
                    onPlay: Logic.play(pieceModel, gameCoord)
                }
            }
        }
    }
    MouseArea {
        visible: isDesktop
        enabled: false // only for setting cursor shape
        anchors.fill: parent
        cursorShape: busyIndicatorRunning ? Qt.BusyCursor : Qt.ArrowCursor
    }

    Settings {
        property alias x: rootWindow.x
        property alias y: rootWindow.y
        property alias width: rootWindow.width
        property alias height: rootWindow.height
        property alias folder: rootWindow.folder
        property alias computerPlays0: rootWindow.computerPlays0
        property alias computerPlays1: rootWindow.computerPlays1
        property alias computerPlays2: rootWindow.computerPlays2
        property alias computerPlays3: rootWindow.computerPlays3
        property alias initComputerColorsOnNewGame: rootWindow.initComputerColorsOnNewGame;
        property alias isRated: rootWindow.isRated
        property alias themeName: rootWindow.themeName
        property alias isDesktop: rootWindow.isDesktop
        property alias wasGenMoveRunning: rootWindow.wasGenMoveRunning
        property alias exportImageWidth: rootWindow.exportImageWidth
        property alias showVariations: gameModel.showVariations
    }
    GameModel {
        id: gameModel

        onPositionAboutToChange: Logic.cancelRunning()
        onPositionChanged: {
            gameDisplay.pickedPiece = null
            if (gameModel.canGoBackward || gameModel.canGoForward
                    || gameModel.moveNumber > 0)
                gameDisplay.setupMode = false
            analyzeGameModel.markCurrentMove(gameModel)
            gameDisplay.dropCommentFocus()
        }
        onInvalidSgfFile: Logic.showInfo(gameModel.lastInputOutputError)
    }
    PlayerModel {
        id: playerModel

        gameVariant: gameModel.gameVariant
        onMoveGenerated: Logic.moveGenerated(move)
        onSearchCallback: gameDisplay.searchCallback(elapsedSeconds, remainingSeconds)
        onIsGenMoveRunningChanged:
            if (isGenMoveRunning) gameDisplay.startSearch()
            else gameDisplay.endSearch()
        Component.onCompleted:
            if (notEnoughMemory())
                Logic.showInfoWithCallback(qsTr("Not enough memory."), Qt.quit)
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

            onAccepted: {
                rootWindow.computerPlays0 = computerColorDialog.computerPlays0
                rootWindow.computerPlays1 = computerColorDialog.computerPlays1
                rootWindow.computerPlays2 = computerColorDialog.computerPlays2
                rootWindow.computerPlays3 = computerColorDialog.computerPlays3
                initComputerColorsOnNewGame = Logic.computerPlaysAny()
                if (! Logic.isComputerToPlay())
                    Logic.cancelRunning()
                else if (! gameModel.isGameOver)
                    Logic.checkComputerMove()
            }
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

        Pentobi.QuestionDialog { }
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
