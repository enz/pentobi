import QtQuick 2.0
import Qt.labs.controls 1.0
import QtQuick.Controls 1.2 as Controls1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import "Main.js" as Logic

Dialog {
    title: qsTr("Initial Rating")
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    onAccepted: {
        ratingModel.setInitialRating(Math.round(slider.value))
        Logic.ratedGameNoVerify()
    }
    onVisibleChanged: if (! visible) gameDisplay.forceActiveFocus() // QTBUG-48456

    Column
    {
        width: Math.min(Screen.pixelDensity * 120, 0.95 * Screen.width)
        spacing: Screen.pixelDensity * 2

        Label {
            text: qsTr("You have not yet played rated games in this game variant. Estimate your playing strength to initialize your rating.")
            width: 0.9 * parent.width
            wrapMode: Text.Wrap
        }
        Row {
            spacing: Screen.pixelDensity * 2

            Label {
                text: qsTr("Beginner")
                anchors.verticalCenter: parent.verticalCenter
            }

            // Don't use Qt Quick Controls 2 Slider yet, it doesn't look good
            // on high DPI devices without enabling Qt::AA_EnableHighDpiScaling
            // and that causes problems as long as we still also use Qt Quick
            // Controls 1 and Dialogs.
            Controls1.Slider {
                id: slider

                width: Screen.pixelDensity * 25
                value: 1000
                minimumValue: 1000; maximumValue: 2000
                stepSize: 100
                anchors.verticalCenter: parent.verticalCenter
            }

            Label {
                text: qsTr("Expert")
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        RowLayout {
            Label {
                text: qsTr("Your initial rating:")
            }
            Label {
                text: Math.round(slider.value)
                font.bold: true
            }
        }
    }
}
