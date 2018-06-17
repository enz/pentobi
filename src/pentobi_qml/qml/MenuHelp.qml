import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic

Menu {
    title: qsTr("&Help")

    MenuItem {
        text: qsTr("Pentobi &Help")
        onTriggered: Logic.help()
    }
    MenuItem {
        text: qsTr("&About Pentobi")
        onTriggered: Logic.about()
    }
}
