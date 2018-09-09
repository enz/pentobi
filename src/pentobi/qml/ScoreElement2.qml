//-----------------------------------------------------------------------------
/** @file pentobi/qml/ScoreElement2.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0

RowLayout {
    id: root

    property color color1
    property color color2
    property bool isFinal
    property bool isAltColor
    property real value
    property alias fontSize: text.font.pixelSize

    spacing: 0

    Rectangle {
        id: point1

        color: color1
        opacity: isAltColor && isFinal ? 0 : 1
        radius: width / 2
        Layout.preferredWidth: 0.7 * fontSize
        Layout.preferredHeight: 0.7 * fontSize
    }
    Rectangle {
        id: point2

        color: isAltColor && isFinal ? color1 : color2
        radius: width / 2
        Layout.preferredWidth: 0.7 * fontSize
        Layout.preferredHeight: 0.7 * fontSize
    }
    // Spacer, which will shrink if the available width is small
    Item {
        Layout.fillWidth: true
        Layout.preferredWidth: 0.14 * fontSize
        Layout.maximumWidth: 0.14 * fontSize
    }
    Text {
        id: text

        text: isAltColor ? ""
                         : isFinal ? "<u>%L1</u>".arg(value) : "%L1".arg(value)
        color: theme.colorText
        opacity: 0.8
        font.preferShaping: false
        // Minimum width to avoid early position change when the number of
        // digits in the score changes from 1 to 2
        Layout.minimumWidth: isAltColor ? 0 : 1.5 * fontSize
    }
}
