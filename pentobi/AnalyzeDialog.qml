//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "main.js" as Logic

PentobiDialog {
    id: root

    footer: DialogButtonBoxOkCancel { }
    onAboutToShow: comboBox.currentIndex = 0
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
            Math.max(Math.min(columnLayout.implicitWidth, root.maxContentWidth),
                     root.minContentWidth)
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            Label {
                id: label

                Layout.fillWidth: true
                text: qsTr("Analysis speed:")
            }
            ComboBox {
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
