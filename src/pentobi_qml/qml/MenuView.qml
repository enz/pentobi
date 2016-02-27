import QtQuick 2.0
import QtQuick.Controls 1.1

Menu {
    title: qsTr("&View")

    MenuItem {
        text: qsTr("Mark &Last Move")
        checkable: true
        checked: gameDisplay.markLastMove
        onTriggered: gameDisplay.markLastMove = checked
    }
    MenuItem {
        text: qsTr("&Animate Pieces")
        checkable: true
        checked: gameDisplay.enableAnimations
        onTriggered: gameDisplay.enableAnimations = checked
    }
}
