//-----------------------------------------------------------------------------
/** @file pentobi/InitialRatingDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "main.js" as Logic

PentobiDialog {
    footer: DialogButtonBoxOkCancel { }
    onAccepted: {
        ratingModel.setInitialRating(Math.round(slider.value))
        Logic.ratedGameNoVerify()
    }

    ColumnLayout
    {
        Item {
            implicitWidth:
                Math.max(Math.min(textLabel.implicitWidth, maxContentWidth),
                         minContentWidth)
            implicitHeight: textLabel.implicitHeight
            Layout.fillWidth: true

            Label {
                id: textLabel

                anchors.fill: parent
                text: qsTr("Initialize your rating for this game variant.")
                wrapMode: Text.Wrap
            }
        }
        RowLayout {
            Layout.topMargin: 0.6 * font.pixelSize

            Label {
                text: qsTr("Initial rating:")
            }
            Label {
                text: Math.round(slider.value)
                font.bold: true
            }
        }
        Slider {
            id: slider

            value: 800
            from: 800; to: 2000; stepSize: 100
            Layout.fillWidth: true
        }
        RowLayout {
            Layout.fillWidth: true

            Label { text: qsTr("Beginner") }
            Item { Layout.fillWidth: true }
            Label { text: qsTr("Expert") }
        }
    }
}
