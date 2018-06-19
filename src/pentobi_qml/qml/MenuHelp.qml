import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic

Menu {
    title: Logic.removeShortcut(qsTr("&Help"))

    MenuItem {
        text: Logic.removeShortcut(qsTr("Pentobi &Help"))
        onTriggered: Logic.help()
    }
    MenuItem {
        text: Logic.removeShortcut(qsTr("&About Pentobi"))
        onTriggered: Logic.about()
    }
}
