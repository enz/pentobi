import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("G&o")
    visible: ! isAndroid || backToMainVar.enabled || gotoMove.enabled

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
}
