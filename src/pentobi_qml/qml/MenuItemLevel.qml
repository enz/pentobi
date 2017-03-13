import QtQuick.Controls 1.1
import "Main.js" as Logic

MenuItem {
    property int level

    enabled: ! isRated
    text: qsTr("Level &%1").arg(level)
    checkable: true
    exclusiveGroup: levelGroup
    checked: playerModel.level === level
    onTriggered: playerModel.level = level
}
