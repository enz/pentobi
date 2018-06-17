import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

RowLayout {
    id: root

    property real buttonPadding: Math.round(1.5 * Screen.pixelDensity)

    spacing: 0

    Label {
        text: {
            if (isRated) return qsTr("Rated")
            if (gameDisplay.setupMode) return qsTr("Leave setup")
            if (gameModel.file === "") return ""
            return Logic.getFileLabel(gameModel.file, gameModel.isModified)
        }
        Layout.fillWidth: true
        Layout.leftMargin: root.height / 10
        renderType: Text.NativeRendering
        color: theme.toolBarTextColor
        elide: Text.ElideRight

        MouseArea {
            anchors.fill: parent
            onClicked:
                if (gameDisplay.setupMode)
                    gameDisplay.setupMode = false
                else if (gameModel.file != "") {
                    if (gameModel.isModified)
                        Logic.showInfo(qsTr("File (modified): %1").arg(gameModel.file))
                    else
                        Logic.showInfo(qsTr("File: %1").arg(gameModel.file))
                }
        }
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-newgame")
        enabled: ! (gameModel.isGameEmpty && gameModel.file === "" && ! gameModel.isModified && ! isRated) && ! gameDisplay.setupMode
        onClicked: Logic.newGame()
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-undo")
        enabled: gameModel.canUndo && ! gameDisplay.setupMode && ! isRated
        onClicked: Logic.undo()
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-computer-colors")
        enabled: ! gameDisplay.setupMode
        onClicked: Logic.showComputerColorDialog()
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("pentobi-play")
        enabled: ! gameModel.isGameOver && ! gameDisplay.setupMode && ! isRated
        onClicked: Logic.computerPlay()
    }
    Pentobi.Button {
        padding: buttonPadding
        imageSource: theme.getImage("menu")
        onClicked: menu.popup()

        Menu {
            id: menu

            MenuGame { }
            MenuGo {
                // Note: it's not good enough to set enabled in MenuGo because
                // properties of invisible items are lazily updated (last
                // tested with Qt 5.11)
                enabled: ! isRated && (gameModel.canGoForward || gameModel.canGoBackward)
            }
            MenuEdit {
                // Note: it's not good enough to set enabled in MenuEdit
                // because properties of invisible items are lazily updated
                // (last tested with Qt 5.11)
                enabled: ! isRated
            }
            MenuView { }
            MenuComputer { }
            MenuTools { }
            MenuHelp { }
        }
    }
}
