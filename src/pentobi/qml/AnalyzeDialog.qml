import QtQuick 2.0
import QtQuick.Layouts 1.1
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

    Item {
        implicitWidth: {
            var w = columnLayout.implicitWidth
            // Avoid too small width because Default style in Qt 5.11 makes
            // footer no wider than content, which can cause elided text on
            // dialog buttons
            w = Math.max(w, font.pixelSize * 18)
            w = Math.min(w, 0.85 * rootWindow.width)
            return w
        }
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

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
}
