import QtQuick.Controls 1.1

MenuItem {
    property int level

    text: "&" + level
    checkable: true
    exclusiveGroup: levelGroup
    checked: {
        switch (gameModel.gameVariant) {
        case "classic_2": return playerModel.levelClassic2 === level
        case "classic_3": return playerModel.levelClassic3 === level
        case "duo": return playerModel.levelDuo === level
        case "trigon": return playerModel.levelTrigon === level
        case "trigon_2": return playerModel.levelTrigon2 === level
        case "trigon_3": return playerModel.levelTrigon3 === level
        case "junior": return playerModel.levelJunior === level
        case "nexos": return playerModel.levelNexos === level
        case "nexos_2": return playerModel.levelNexos2 === level
        default: return playerModel.levelClassic === level
        }
    }
    onTriggered: {
        switch (gameModel.gameVariant) {
        case "classic_2": playerModel.levelClassic2 = level; break
        case "classic_3": playerModel.levelClassic3 = level; break
        case "duo": playerModel.levelDuo = level; break
        case "trigon": playerModel.levelTrigon = level; break
        case "trigon_2": playerModel.levelTrigon2 = level; break
        case "trigon_3": playerModel.levelTrigon3 = level; break
        case "junior": playerModel.levelJunior = level; break
        case "nexos": playerModel.levelNexos = level; break
        case "nexos_2": playerModel.levelNexos2 = level; break
        default: playerModel.levelClassic = level
        }
    }
}
