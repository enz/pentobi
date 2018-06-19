import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic

Menu {
    title: Logic.removeShortcut(qsTr("G&o"))
    cascade: false

    MenuItem {
        id: gotoMove

        text: Logic.removeShortcut(qsTr("&Go to Move..."))
        enabled: gameModel.moveNumber + gameModel.movesLeft > 1
        onTriggered: gotoMoveDialog.open()
    }
    MenuItem {
        id: backToMainVar

        text: Logic.removeShortcut(qsTr("Back to &Main Variation"))
        enabled: ! gameModel.isMainVar
        onTriggered: gameModel.backToMainVar()
    }
    MenuItem {
        id: beginningOfBranch

        text: Logic.removeShortcut(qsTr("Beginning of Bran&ch"))
        enabled: gameModel.hasEarlierVar
        onTriggered: gameModel.gotoBeginningOfBranch()
    }
    MenuSeparator { }
    MenuItem {
        id: findNextComment

        text: Logic.removeShortcut(qsTr("Find Next &Comment"))
        enabled: gameModel.canGoForward || gameModel.canGoBackward
        onTriggered: Logic.findNextComment()
    }
}
