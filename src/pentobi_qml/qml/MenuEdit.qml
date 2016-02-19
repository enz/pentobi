import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

Menu {
    title: qsTr("&Edit")

    MenuItem {
        text: qsTr("Make &Main Variation")
        enabled: ! gameModel.isMainVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.makeMainVar()
    }
    MenuItem {
        text: qsTr("Move Variation &Up")
        enabled: gameModel.hasPrevVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.moveUpVar()
    }
    MenuItem {
        text: qsTr("Move Variation &Down")
        enabled: gameModel.hasNextVar
        visible: ! isAndroid || enabled
        onTriggered: gameModel.moveDownVar()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Delete All Variations")
        enabled: gameModel.hasVariations
        visible: ! isAndroid || enabled
        onTriggered: Logic.deleteAllVar()
    }
    MenuItem {
        text: qsTr("&Truncate")
        enabled: gameModel.canGoBackward
        visible: ! isAndroid || enabled
        onTriggered: Logic.truncate()
    }
    MenuItem {
        text: qsTr("Truncate &Children")
        enabled: gameModel.canGoForward
        visible: ! isAndroid || enabled
        onTriggered: Logic.truncateChildren()
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Next Color")
        onTriggered: gameModel.nextColor()
    }
}
