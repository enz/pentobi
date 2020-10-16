//-----------------------------------------------------------------------------
/** @file pentobi/qml/Main.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQml 2.2
import QtQuick 2.11
import QtQuick.Controls 2.3
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

    property alias gameView: gameViewLoader.item

    // If the user manually disabled all computer colors in the dialog, we
    // assume that they want to edit games rather than play, and we will not
    // initialize the computer colors on New Game but only clear the board.
    property bool initComputerColorsOnNewGame: true

    property bool isAndroid: Qt.platform.os === "android"
    // "system" theme would be a better default on desktop with Fusion style
    // but we currently don't use Fusion (see comment in Main.cpp about
    // QTBUG-77107)
    property string themeName: isAndroid ? "dark" : "light"
    property QtObject theme: Logic.createTheme(themeName)
    property url folder: androidUtils.getDefaultFolder()

    property real defaultWidth:
        isAndroid ? Screen.desktopAvailableWidth
                  : Math.min(Screen.desktopAvailableWidth, 1164)
    property real defaultHeight:
        isAndroid ? Screen.desktopAvailableHeight
                  : Math.min(Screen.desktopAvailableHeight,
                             defaultWidth * 662 / 1164)

    property int exportImageWidth: 420
    property bool busyIndicatorRunning: lengthyCommand.isRunning
                                        || playerModel.isGenMoveRunning
                                        || analyzeGameModel.isRunning
    property bool showToolBar: true

    minimumWidth: isDesktop ? 481 : 240
    minimumHeight: isDesktop ? 303 : 301
    color: theme.colorBackground
    title: Logic.getWindowTitle(gameModel.file, gameModel.isModified)
    onClosing: if ( ! Logic.quit()) close.accepted = false
    Component.onCompleted: Logic.init()
    Component.onDestruction: Logic.cancelRunning()

    MouseArea {
        anchors.fill: parent
        onClicked: gameView.dropCommentFocus()
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
        id: gameViewLoader

        anchors {
            left: parent.left
            right: parent.right
            top: toolBar.visible ? toolBar.bottom : parent.top
            bottom: parent.bottom
            margins: isDesktop ? 2 : 0
        }
        source: isDesktop ? "GameViewDesktop.qml" : "GameViewMobile.qml"

        Connections {
            target: gameViewLoader.item

            function onPlay(pieceModel, gameCoord) {
                Logic.play(pieceModel, gameCoord)
            }
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
        // Changed alias name from themeName to theme to ignore old saved
        // settings from Pentobi <=17.1 after changing default theme on desktop
        // (see comment in Main.cpp about QTBUG-77107)
        property alias theme: rootWindow.themeName
        property alias exportImageWidth: rootWindow.exportImageWidth
        property alias showToolBar: rootWindow.showToolBar
        property alias showVariations: gameModel.showVariations
    }
    GameModel {
        id: gameModel

        onPositionAboutToChange: Logic.cancelRunning(true)
        onPositionChanged: {
            gameView.pickedPiece = null
            if (gameModel.canGoBackward || gameModel.canGoForward
                    || gameModel.moveNumber > 0)
                gameView.setupMode = false
            analyzeGameModel.markCurrentMove(gameModel)
            gameView.onPositionChanged()
            gameView.dropCommentFocus()
        }
        onInvalidSgfFile: Logic.showInfo(gameModel.getError())
    }
    PlayerModel {
        id: playerModel

        gameVariant: gameModel.gameVariant
        onMoveGenerated: Logic.moveGenerated(move)
        onSearchCallback: gameView.searchCallback(elapsedSeconds, remainingSeconds)
        onIsGenMoveRunningChanged:
            if (isGenMoveRunning) gameView.startSearch()
            else gameView.endSearch()
        Component.onCompleted:
            if (notEnoughMemory())
                Logic.showFatal(qsTr("Not enough memory"))
    }
    AnalyzeGameModel {
        id: analyzeGameModel

        onIsRunningChanged: if (! isRunning) gameView.endAnalysis()
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
    Timer {
        id: pressBackTwice

        interval: 2500
    }
    Connections {
        target: Qt.application
        enabled: isAndroid

        function onStateChanged() {
            if (Qt.application.state === Qt.ApplicationSuspended)
                Logic.autoSaveNoVerify()
        }
    }

    Action {
        id: actionBackToMainVar

        shortcut: "Ctrl+M"
        text: qsTr("Main Variation")
        enabled: ! isRated && ! gameModel.isMainVar
        onTriggered: gameModel.backToMainVar()
    }
    Action {
        id: actionBackward

        shortcut: "Ctrl+Left"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBackward()
    }
    Action {
        id: actionBackward10

        shortcut: "Ctrl+Shift+Left"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBackward10()
    }
    Action {
        id: actionBeginning

        shortcut: "Ctrl+Home"
        enabled: gameModel.canGoBackward && ! isRated
        onTriggered: gameModel.goBeginning()
    }
    Action {
        id: actionForward

        shortcut: "Ctrl+Right"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goForward()
    }
    Action {
        id: actionForward10

        shortcut: "Ctrl+Shift+Right"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goForward10()
    }
    Action {
        id: actionEnd

        shortcut: "Ctrl+End"
        enabled: gameModel.canGoForward && ! isRated
        onTriggered: gameModel.goEnd()
    }
    Action {
        id: actionPrevVar

        shortcut: "Ctrl+Up"
        enabled: gameModel.hasPrevVar && ! isRated
        onTriggered: gameModel.goPrevVar()
    }
    Action {
        id: actionNextVar

        shortcut: "Ctrl+Down"
        enabled: gameModel.hasNextVar && ! isRated
        onTriggered: gameModel.goNextVar()
    }
    Action {
        id: actionBeginningOfBranch

        shortcut: "Ctrl+B"
        text: qsTr("Beginning of Branch")
        enabled: ! isRated && gameModel.hasEarlierVar
        onTriggered: gameModel.gotoBeginningOfBranch()
    }
    Action {
        id: actionComment

        shortcut: "Ctrl+T"
        text: qsTr("Comment")
        checkable: true
        checked: gameView.isCommentVisible
        onTriggered:
            if (isDesktop)
                gameView.setCommentVisible(checked)
            else {
                if (checked)
                    gameView.showComment()
                else
                    gameView.showPieces()
            }
    }
    Action {
        id: actionComputerSettings

        shortcut: "Ctrl+U"
        //: Menu item Computer/Settings
        text: qsTr("Settings")
        onTriggered: computerDialog.open()
    }
    Action {
        id: actionFindMove

        shortcut: "Ctrl+H"
        text: qsTr("Find Move")
        enabled: ! gameModel.isGameOver
        onTriggered: gameView.showMove(gameModel.findMoveNext())
    }
    Action {
        id: actionNextComment

        shortcut: "Ctrl+E"
        text: qsTr("Next Comment")
        enabled: ! isRated && (gameModel.canGoForward || gameModel.canGoBackward)
        onTriggered: Logic.findNextComment()
    }
    Action {
        id: actionFullscreen

        shortcut: "F11"
        text: qsTr("Fullscreen")
        checkable: true
        checked: visibility === Window.FullScreen
        onTriggered: {
            if (visibility !== Window.FullScreen)
                visibility = Window.FullScreen
            else
                visibility = Window.AutomaticVisibility
        }
    }
    Action {
        id: actionGameInfo

        shortcut: "Ctrl+I"
        text: qsTr("Game Info")
        onTriggered: gameInfoDialog.open()
    }
    Action {
        id: actionGotoMove

        shortcut: "Ctrl+G"
        text: qsTr("Move Number…")
        enabled: ! isRated && (gameModel.moveNumber + gameModel.movesLeft >= 1)
        onTriggered: gotoMoveDialog.open()
    }
    Action {
        id: actionHelp

        shortcut: "F1"
        text: qsTr("Pentobi Help")
        onTriggered: Logic.help()
    }
    Action {
        id: actionNew

        shortcut: "Ctrl+N"
        text: qsTr("New")
        // The conditions canGoBackward/Forward are not really needed because
        // they can only be true if either a file was loaded or the game is
        // modified, but we add them to make sure that the New Game button is
        // always enabled when needed even if there is an inconsistency in the
        // other conditions due to a bug in Pentobi.
        enabled: gameView.setupMode || gameModel.isModified
                 || gameModel.file !== "" || isRated || gameModel.canGoBackward
                 || gameModel.canGoForward
                 || analyzeGameModel.elements.length !== 0
        onTriggered: Logic.newGame()
    }
    Action {
        id: actionNewRated

        shortcut: "Ctrl+Shift+N"
        text: qsTr("Rated Game")
        enabled: ! isRated
        onTriggered: Logic.ratedGame()
    }
    Action {
        id: actionOpen

        shortcut: "Ctrl+O"
        text: qsTr("Open…")
        onTriggered: Logic.open()
    }
    Action {
        id: actionPlay

        shortcut: "Ctrl+L"
        text: qsTr("Play")
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: Logic.computerPlay()
    }
    Action {
        id: actionPlaySingle

        shortcut: "Ctrl+Shift+L"
        //: Play a single move
        text: qsTr("Play Move")
        enabled: ! gameModel.isGameOver && ! isRated
        onTriggered: { isPlaySingleMoveRunning = true; Logic.genMove() }
    }
    Action {
        id: actionQuit

        shortcut: "Ctrl+Q"
        text: qsTr("Quit")
        onTriggered: close()
    }
    Action {
        id: actionSave

        shortcut: "Ctrl+S"
        text: qsTr("Save")
        enabled: gameModel.isModified
        onTriggered: if (gameModel.file !== "") Logic.save(); else Logic.saveAs()
    }
    Action {
        id: actionSaveAs

        shortcut: "Ctrl+Shift+S"
        text: qsTr("Save As…")
        enabled: gameModel.isModified || gameModel.file !== ""
        onTriggered: Logic.saveAs()
    }
    Action {
        id: actionStop

        text: qsTr("Stop")
        enabled: (playerModel.isGenMoveRunning
                  || delayedCheckComputerMove.running
                  || analyzeGameModel.isRunning)
                 && ! isRated
        onTriggered: Logic.cancelRunning(true)
    }
    Action {
        id: actionUndo

        text: qsTr("Undo Move")
        enabled: gameModel.canUndo && ! gameView.setupMode && ! isRated
        onTriggered: Logic.undo()
    }
    Instantiator {
        model: [ "1", "2", "A", "C", "E", "F", "G", "H", "I", "J", "L",
            "N", "O", "P", "S", "T", "U", "V", "W", "X", "Y", "Z" ]

        Shortcut {
            sequence: modelData
            onActivated: Logic.pickNamedPiece(modelData)
        }
    }
    Shortcut {
        sequence: "Back"
        enabled: isAndroid
        onActivated: {
            if (visibility === Window.FullScreen)
                visibility = Window.AutomaticVisibility
            else if (pressBackTwice.running)
                close()
            else {
                Logic.showTemporaryMessage(qsTr("Press back again to exit"))
                pressBackTwice.start()
            }
        }
    }
    Shortcut {
        sequence: "Return"
        enabled: ! isAndroid
        onActivated: gameView.playPickedPiece()
    }
    Shortcut {
        sequence: "Escape"
        onActivated:
            if (gameView.pickedPiece)
                gameView.pickedPiece = null
            else if (visibility === Window.FullScreen)
                visibility = Window.AutomaticVisibility
    }
    Shortcut {
        sequence: "Ctrl+Shift+H"
        enabled: ! gameModel.isGameOver
        onActivated: gameView.showMove(gameModel.findMovePrevious())
    }
    Shortcut {
        sequence: "Down"
        onActivated: gameView.shiftPiece(0, 1)
    }
    Shortcut {
        sequence: "Shift+Down"
        onActivated: gameView.shiftPieceFast(0, 1)
    }
    Shortcut {
        sequence: "Left"
        onActivated: gameView.shiftPiece(-1, 0)
    }
    Shortcut {
        sequence: "Shift+Left"
        onActivated: gameView.shiftPieceFast(-1, 0)
    }
    Shortcut {
        sequence: "Right"
        onActivated: gameView.shiftPiece(1, 0)
    }
    Shortcut {
        sequence: "Shift+Right"
        onActivated: gameView.shiftPieceFast(1, 0)
    }
    Shortcut {
        sequence: "Up"
        onActivated: gameView.shiftPiece(0, -1)
    }
    Shortcut {
        sequence: "Shift+Up"
        onActivated: gameView.shiftPieceFast(0, -1)
    }
    Shortcut {
        enabled: gameView.pickedPiece
        sequence: "Space"
        onActivated: gameView.pickedPiece.pieceModel.nextOrientation()
    }
    Shortcut {
        sequence: "+"
        onActivated: Logic.nextPiece()
    }
    Shortcut {
        sequence: "Alt+M"
        onActivated: toolBar.clickMenuButton()
    }
    Shortcut {
        enabled: gameView.pickedPiece
        sequence: "Shift+Space"
        onActivated: gameView.pickedPiece.pieceModel.previousOrientation()
    }
    Shortcut {
        sequence: "-"
        onActivated: Logic.prevPiece()
    }
}
