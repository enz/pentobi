import QtQuick.Controls 2.2
import "." as Pentobi

Pentobi.MenuItem {
    property int level

    enabled: ! isRated
    text: qsTr("Level &%1").arg(level)
    checkable: true
    autoExclusive: true
    checked: playerModel.level === level
    onTriggered: {
        playerModel.level = level
    }
}
