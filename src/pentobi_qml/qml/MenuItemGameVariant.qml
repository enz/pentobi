import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

MenuItem {
    property string gameVariant

    checkable: true
    checked: gameModel.gameVariant == gameVariant
    exclusiveGroup: groupGameVariant
    onTriggered: Logic.changeGameVariant(gameVariant, true)
}
