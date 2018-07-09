import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.MenuItem {
    property string gameVariant

    checkable: true
    checked: gameModel.gameVariant == gameVariant
    onTriggered: {
        Logic.changeGameVariant(gameVariant)
        // Trigger an update of checked in all game variant items in case
        // the user aborted the game variant change in the discard game dialog.
        gameModel.gameVariantChanged()
    }
    ButtonGroup.group: groupGameVariant
}
