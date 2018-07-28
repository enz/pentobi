//-----------------------------------------------------------------------------
/** @file pentobi/qml/AnalyzeDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Dialog {
    footer: DialogButtonBoxOkCancel { }
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
        implicitWidth: Math.min(columnLayout.implicitWidth,
                                0.9 * rootWindow.width)
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
