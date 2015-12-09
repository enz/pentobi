import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("&Edit")
    visible: ! isAndroid || makeMainVar.enabled || moveVarUp.enabled ||
             moveVarDown.enabled

    MenuItem {
        id: makeMainVar

        text: qsTr("Make &Main Variation")
        enabled: ! gameModel.isMainVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.makeMainVar()
    }
    MenuItem {
        id: moveVarUp

        text: qsTr("Move Variation &Up")
        enabled: gameModel.hasPrevVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.moveUpVar()
    }
    MenuItem {
        id: moveVarDown

        text: qsTr("Move Variation &Down")
        enabled: gameModel.hasNextVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.moveDownVar()
    }
}
