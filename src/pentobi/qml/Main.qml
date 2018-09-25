//-----------------------------------------------------------------------------
/** @file pentobi/qml/Main.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.11
import QtQuick.Controls 2.2
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

    property alias gameDisplay: gameDisplayLoader.item

    // If the user manually disabled all computer colors in the dialog, we
    // assume that they want to edit games rather than play, and we will not
    // initialize the computer colors on New Game but only clear the board.
    property bool initComputerColorsOnNewGame: true

    property bool isAndroid: Qt.platform.os === "android"
    property string themeName: isAndroid ? "dark" : "system"
    property QtObject theme: Logic.createTheme(themeName)
    property url folder: androidUtils.getDefaultFolder()

    property real defaultWidth:
        isAndroid ? Screen.desktopAvailableWidth
                  : Math.min(Screen.desktopAvailableWidth, 1188)
    property real defaultHeight:
        isAndroid ? Screen.desktopAvailableHeight
                  : Math.min(Screen.desktopAvailableHeight, 663)

    property int exportImageWidth: 420
    property bool busyIndicatorRunning: lengthyCommand.isRunning
                                        || playerModel.isGenMoveRunning
                                        || analyzeGameModel.isRunning
    property bool showToolBar: true

    property Actions actions: Actions { }

    property var dialogs: []

    minimumWidth: isDesktop ? 481 : 240
    minimumHeight: isDesktop ? 303 : 301
    color: theme.colorBackground
    title: Logic.getWindowTitle(gameModel.file, gameModel.isModified)
    onClosing: if ( ! Logic.quit()) close.accepted = false
    Component.onCompleted: Logic.init()

    MouseArea {
        anchors.fill: parent
        onClicked: gameDisplay.dropCommentFocus()
    }
    Pentobi.ToolBar {
        id: toolBar

        visible: isDesktop || visibility !== Window.FullScreen
        showContent: ! isDesktop || showToolBar
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: isDesktop ? 2 : 0
        }
    }
    Loader {
        id: gameDisplayLoader

        anchors {
            left: parent.left
            right: parent.right
            top: toolBar.visible ? toolBar.bottom : parent.top
            bottom: parent.bottom
            margins: isDesktop ? 2 : 0
        }
        source:
            isDesktop ? "GameDisplayDesktop.qml" : "GameDisplayMobile.qml"

        Connections {
            target: gameDisplayLoader.item
            onPlay: Logic.play(pieceModel, gameCoord)
        }
    }
    MouseArea {
        visible: isDesktop
        acceptedButtons: Qt.NoButton // only for setting cursor shape
        anchors.fill: parent
        cursorShape: busyIndicatorRunning ? Qt.BusyCursor : Qt.ArrowCursor
    }
    Settings {
        id: settings

        property real x: (Screen.width - defaultWidth) / 2
        property real y: (Screen.height - defaultHeight) / 2
        property real width: defaultWidth
        property real height: defaultHeight
        property int visibility
        property alias folder: rootWindow.folder
        property alias themeName: rootWindow.themeName
        property alias exportImageWidth: rootWindow.exportImageWidth
        property alias showToolBar: rootWindow.showToolBar
        property alias showVariations: gameModel.showVariations
    }
    GameModel {
        id: gameModel

        onPositionAboutToChange: Logic.cancelRunning(true)
        onPositionChanged: {
            gameDisplay.pickedPiece = null
            if (gameModel.canGoBackward || gameModel.canGoForward
                    || gameModel.moveNumber > 0)
                gameDisplay.setupMode = false
            analyzeGameModel.markCurrentMove(gameModel)
            gameDisplay.dropCommentFocus()
        }
        onInvalidSgfFile: Logic.showInfo(gameModel.getError())
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
                Logic.showFatal(qsTr("Not enough memory"))
    }
    AnalyzeGameModel {
        id: analyzeGameModel

        onIsRunningChanged: if (! isRunning) gameDisplay.endAnalysis()
    }
    RatingModel {
        id: ratingModel

        gameVariant: gameModel.gameVariant
    }
    AndroidUtils { id: androidUtils }
    SyncSettings { id: syncSettings }
    DialogLoader { id: aboutDialog; url: "AboutDialog.qml" }
    DialogLoader { id: computerDialog; url: "ComputerDialog.qml" }
    DialogLoader { id: fatalMessage; url: "FatalMessage.qml" }
    DialogLoader { id: gameVariantDialog; url: "GameVariantDialog.qml" }
    DialogLoader { id: gameInfoDialog; url: "GameInfoDialog.qml" }
    DialogLoader { id: initialRatingDialog; url: "InitialRatingDialog.qml" }
    DialogLoader { id: newFolderDialog; url: "NewFolderDialog.qml" }
    DialogLoader { id: openDialog; url: "OpenDialog.qml" }
    DialogLoader { id: exportImageDialog; url: "ExportImageDialog.qml" }
    DialogLoader { id: imageSaveDialog; url: "ImageSaveDialog.qml" }
    DialogLoader { id: asciiArtSaveDialog; url: "AsciiArtSaveDialog.qml" }
    DialogLoader { id: gotoMoveDialog; url: "GotoMoveDialog.qml" }
    DialogLoader { id: ratingDialog; url: "RatingDialog.qml" }
    DialogLoader { id: saveDialog; url: "SaveDialog.qml" }
    DialogLoader { id: infoMessage; url: "MessageDialog.qml" }
    DialogLoader { id: questionMessage; url: "QuestionDialog.qml" }
    DialogLoader { id: analyzeDialog; url: "AnalyzeDialog.qml" }
    DialogLoader { id: appearanceDialog; url: "AppearanceDialog.qml" }
    DialogLoader { id: moveAnnotationDialog; url: "MoveAnnotationDialog.qml" }
    Loader { id: helpWindow }

    // Used to delay calls to Logic.checkComputerMove such that the computer
    // starts thinking and the busy indicator is visible after the current move
    // placement animation has finished
    Timer {
        id: delayedCheckComputerMove

        interval: 500
        onTriggered: Logic.checkComputerMove()
    }

    // Delay lengthy blocking function calls such that busy indicator is visible
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
        enabled: isAndroid
        onStateChanged:
            if (Qt.application.state === Qt.ApplicationSuspended)
                Logic.autoSaveNoVerify()
    }
}
