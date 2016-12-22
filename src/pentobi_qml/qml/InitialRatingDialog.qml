import QtQuick 2.0
import QtQuick.Controls 2.1
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import "Main.js" as Logic

Dialog {
    title: qsTr("Initial Rating")
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    onAccepted: {
        ratingModel.setInitialRating(Math.round(slider.value))
        gameDisplay.forceActiveFocus() // QTBUG-48456
        Logic.ratedGameNoVerify()
    }
    onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456

    Column
    {
        width: Math.min(Screen.pixelDensity * 120, 0.95 * Screen.width)
        spacing: Screen.pixelDensity * 2

        Label {
            text: qsTr("You have not yet played rated games in this game variant. Estimate your playing strength to initialize your rating.")
            width: 0.9 * parent.width
            wrapMode: Text.Wrap
        }
        RowLayout {
            Label { text: qsTr("Beginner") }
            Slider {
                id: slider

                value: 1000
                from: 1000; to: 2000
                stepSize: 100
                onPositionChanged: value = 1000 + 1000 * position
            }
            Label { text: qsTr("Expert") }
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
