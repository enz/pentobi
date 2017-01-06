import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.1

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
    MenuSeparator { }
    MenuItem {
        text: qsTr("&Fullscreen")
        checkable: true
        checked: visibility === Window.FullScreen
        onTriggered:
            if (visibility !== Window.FullScreen)
                visibility = Window.FullScreen
            else
                visibility = Window.AutomaticVisibility
    }
}
