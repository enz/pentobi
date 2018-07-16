import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Dialog {
    title: isDesktop ? qsTr("Analyze Game") : ""
    footer: OkCancelButtons { }
    onAccepted: {
        var nuSimulations
        switch (comboBox.currentIndex) {
        case 2: nuSimulations = 75000; break
        case 1: nuSimulations = 15000; break
        default: nuSimulations = 3000; break
        }
        Logic.analyzeGame(nuSimulations)
    }

    Column {
        spacing: 0.3 * font.pixelSize
        width: Math.min(15 * font.pixelSize, 0.9 * rootWindow.width)

        Label {
            id: label

            width: parent.width
            text: qsTr("Analysis speed:")
        }
        ComboBox {
            id: comboBox

            width: parent.width
            model: isAndroid ? [ qsTr("Fast"), qsTr("Normal") ]
                             : [ qsTr("Fast"), qsTr("Normal"), qsTr("Slow") ]
        }
    }
}
