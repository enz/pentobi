import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

MenuItem {
    property string gameVariant

    checkable: true
    checked: gameModel.gameVariant == gameVariant
    exclusiveGroup: groupGameVariant
    onTriggered: {
        Logic.changeGameVariant(gameVariant)
        // Trigger an update of checked in all game variant items in case
        // the user aborted the game variant change in the discard game dialog.
        gameModel.gameVariantChanged()
    }
}
