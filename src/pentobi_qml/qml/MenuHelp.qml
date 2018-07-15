import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: qsTr("&Help")

    Pentobi.MenuItem { action: actions.actionHelp }
    Pentobi.MenuItem {
        text: qsTr("&About Pentobi")
        onTriggered: Logic.about()
    }
}
