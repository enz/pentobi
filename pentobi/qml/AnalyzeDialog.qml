//-----------------------------------------------------------------------------
/** @file pentobi/qml/AnalyzeDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import "Main.js" as Logic

PentobiDialog {
    footer: DialogButtonBoxOkCancel { }
    onOpened: comboBox.currentIndex = 0
    onAccepted: {
        var nuSimulations
        switch (comboBox.currentIndex) {
        case 2: nuSimulations = 75000; break
        case 1: nuSimulations = 15000; break
        default: nuSimulations = 3000
        }
        Logic.analyzeGame(nuSimulations)
    }

    Item {
        implicitWidth:
            Math.max(Math.min(columnLayout.implicitWidth, maxContentWidth),
                     minContentWidth)
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            Label {
                id: label

                Layout.fillWidth: true
                text: qsTr("Analysis speed:")
            }
            PentobiComboBox {
                id: comboBox

                model:
                    isAndroid ? [ qsTr("Fast"), qsTr("Normal") ]
                              : [ qsTr("Fast"), qsTr("Normal"), qsTr("Slow") ]
                Layout.fillWidth: true
                Layout.preferredWidth: font.pixelSize * 15
            }
        }
    }
}
