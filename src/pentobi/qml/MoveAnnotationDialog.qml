import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Dialog {
    id: root

    title: isDesktop ? qsTr("Move Annotation") : ""
    footer: OkCancelButtons { }
    onOpened: {
        if (gameModel.moveAnnotation === "")
            comboBox.currentIndex = 0;
        else if (gameModel.moveAnnotation === "!!")
            comboBox.currentIndex = 1;
        else if (gameModel.moveAnnotation === "!")
            comboBox.currentIndex = 2;
        else if (gameModel.moveAnnotation === "!?")
            comboBox.currentIndex = 3;
        else if (gameModel.moveAnnotation === "?!")
            comboBox.currentIndex = 4;
        else if (gameModel.moveAnnotation === "?")
            comboBox.currentIndex = 5;
        else if (gameModel.moveAnnotation === "??")
            comboBox.currentIndex = 6;
    }
    onAccepted: {
        switch (comboBox.currentIndex) {
        case 0: gameModel.moveAnnotation = ""; break
        case 1: gameModel.moveAnnotation = "!!"; break
        case 2: gameModel.moveAnnotation = "!"; break
        case 3: gameModel.moveAnnotation = "!?"; break
        case 4: gameModel.moveAnnotation = "?!"; break
        case 5: gameModel.moveAnnotation = "?"; break
        case 6: gameModel.moveAnnotation = "??"; break
        }
    }

    Item {
        implicitWidth: {
            // Qt 5.11 doesn't correctly handle dialog sizing if dialog (incl.
            // frame) is wider than window and Default style never makes footer
            // wider than content (potentially eliding button texts).
            var w = columnLayout.implicitWidth
            w = Math.max(w, font.pixelSize * 18)
            w = Math.min(w, 0.9 * rootWindow.width)
            return w
        }
        implicitHeight: columnLayout.implicitHeight

        ColumnLayout {
            id: columnLayout

            anchors.fill: parent

            Label { text: gameModel.positionInfo }
            Item { Layout.preferredHeight: 0.5 * font.pixelSize }
            Label { text: qsTr("Annotation:") }
            ComboBox {
                id: comboBox

                model: [
                    qsTr("None"),
                    qsTr("Very good"),
                    qsTr("Good"),
                    qsTr("Interesting"),
                    qsTr("Doubtful"),
                    qsTr("Bad"),
                    qsTr("Very Bad")
                ]
            }
        }
    }
}
