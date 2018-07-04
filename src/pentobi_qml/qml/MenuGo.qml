import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: Logic.removeShortcut(qsTr("G&o"))

    MenuItem { action: actions.actionGoto }
    MenuItem { action: actions.actionBackToMainVar }
    MenuItem {
        text: Logic.removeShortcut(qsTr("Beginning of Bran&ch"))
        enabled: gameModel.hasEarlierVar
        onTriggered: gameModel.gotoBeginningOfBranch()
    }
    MenuSeparator { }
    MenuItem {
        text: Logic.removeShortcut(qsTr("Find Next &Comment"))
        enabled: gameModel.canGoForward || gameModel.canGoBackward
        onTriggered: Logic.findNextComment()
    }
}
