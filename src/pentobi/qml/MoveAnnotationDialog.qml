//-----------------------------------------------------------------------------
/** @file pentobi/qml/MoveAnnotationDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Dialog {
    id: root

    property int moveNumber

    footer: DialogButtonBoxOkCancel { }
    onOpened: {
        var annotation = gameModel.getMoveAnnotation(moveNumber)
        if (annotation === "")
            comboBox.currentIndex = 0;
        else if (annotation === "!!")
            comboBox.currentIndex = 1;
        else if (annotation === "!")
            comboBox.currentIndex = 2;
        else if (annotation === "!?")
            comboBox.currentIndex = 3;
        else if (annotation === "?!")
            comboBox.currentIndex = 4;
        else if (annotation === "?")
            comboBox.currentIndex = 5;
        else if (annotation === "??")
            comboBox.currentIndex = 6;
    }
    onAccepted: {
        var annotation
        switch (comboBox.currentIndex) {
        case 0: annotation = ""; break
        case 1: annotation = "!!"; break
        case 2: annotation = "!"; break
        case 3: annotation = "!?"; break
        case 4: annotation = "?!"; break
        case 5: annotation = "?"; break
        case 6: annotation = "??"; break
        }
        gameModel.setMoveAnnotation(moveNumber, annotation)
    }

    Item {
        implicitWidth:
            Math.max(Math.min(columnLayout.implicitWidth, maxContentWidth),
                     minContentWidth)
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            Label { text: qsTr("Move %1").arg(moveNumber) }
            ComboBox {
                id: comboBox

                model: [
                    qsTr("No annotation"),
                    qsTr("Very good"),
                    qsTr("Good"),
                    qsTr("Interesting"),
                    qsTr("Doubtful"),
                    qsTr("Bad"),
                    qsTr("Very Bad")
                ]
                Layout.preferredWidth: 15 * font.pixelSize
                Layout.fillWidth: true
            }
        }
    }
}
