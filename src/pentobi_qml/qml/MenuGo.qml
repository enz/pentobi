import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("G&o")
    enabled:  ! isRated &&
              (backToMainVar.enabled || gotoMove.enabled
               || beginningOfBranch.enabled || findNextComment.enabled)
    visible: ! isAndroid || enabled

    MenuItem {
        id: gotoMove

        text: qsTr("&Go to Move...")
        enabled: gameModel.moveNumber + gameModel.movesLeft > 1
        visible: ! isAndroid || enabled
        onTriggered: gotoMoveDialog.open()
    }
    MenuItem {
        id: backToMainVar

        text: qsTr("Back to &Main Variation")
        enabled: ! gameModel.isMainVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.backToMainVar()
    }
    MenuItem {
        id: beginningOfBranch

        text: qsTr("Beginning of Bran&ch")
        enabled: gameModel.hasEarlierVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.gotoBeginningOfBranch()
    }
    MenuSeparator { }
    MenuItem {
        id: findNextComment

        text: qsTr("Find Next &Comment")
        enabled: gameModel.canGoForward || gameModel.canGoBackward
        visible: ! isAndroid || enabled
        onTriggered: Logic.findNextComment()
    }
}
