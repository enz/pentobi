import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import "." as Pentobi
import "Main.js" as Logic

Pentobi.Dialog {
    title: isDesktop ? qsTr("Initial Rating") : ""
    footer: OkCancelButtons { }
    onAccepted: {
        ratingModel.setInitialRating(Math.round(slider.value))
        Logic.ratedGameNoVerify()
    }

    Column
    {
        width: Math.min(initialRatingLabel.height * 25, 0.9 * rootWindow.width)
        spacing: Screen.pixelDensity * 2

        Label {
            text: qsTr("You have not yet played rated games in this game variant. Estimate your playing strength to initialize your rating.")
            width: 0.9 * parent.width
            wrapMode: Text.Wrap
        }
        RowLayout {
            Label {
                id: initialRatingLabel
                text: qsTr("Your initial rating:")
            }
            Label {
                text: Math.round(slider.value)
                font.bold: true
            }
        }
        Slider {
            id: slider

            width: 0.9 * parent.width
            value: 1000
            from: 1000; to: 2000; stepSize: 100
        }
        RowLayout {
            width: parent.width

            Label {
                text: qsTr("Beginner")
            }
            Label {
                text: qsTr("Expert")
            }
        }
    }
}
