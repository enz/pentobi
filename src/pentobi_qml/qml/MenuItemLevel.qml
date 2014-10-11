import QtQuick 2.0
import QtQuick.Controls 1.1
import "Main.js" as Logic

MenuItem {
    property int level

    text: level
    checkable: true
    exclusiveGroup: levelGroup
    checked: {
        switch (boardModel.gameVariant) {
        case "classic_2": return playerModel.levelClassic2 == level
        case "duo": return playerModel.levelDuo == level
        case "trigon": return playerModel.levelTrigon == level
        case "trigon_2": return playerModel.levelTrigon2 == level
        case "trigon_3": return playerModel.levelTrigon3 == level
        case "junior": return playerModel.levelJunior == level
        default: return playerModel.levelClassic == level
        }
    }
    onTriggered: {
        switch (boardModel.gameVariant) {
        case "classic_2": playerModel.levelClassic2 = level; break
        case "duo": playerModel.levelDuo = level; break
        case "trigon": playerModel.levelTrigon = level; break
        case "trigon_2": playerModel.levelTrigon2 = level; break
        case "trigon_3": playerModel.levelTrigon3 = level; break
        case "junior": playerModel.levelJunior = level; break
        default: playerModel.levelClassic = level
        }
    }
}
