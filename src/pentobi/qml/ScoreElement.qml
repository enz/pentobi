//-----------------------------------------------------------------------------
/** @file pentobi/qml/ScoreElement.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0

RowLayout {
    id: root

    property alias color: point.color
    property bool isFinal
    property real value
    property real bonus
    property int fontSize

    spacing: 0

    Rectangle {
        id: point

        radius: width / 2
        Layout.preferredWidth: 0.7 * fontSize
        Layout.preferredHeight: 0.7 * fontSize
    }
    Text {
        id: text

        text: ! isFinal ?
                  "%L1".arg(value) :
                  "%1<u>%L2</u>".arg(bonus > 0 ? "★" : "").arg(value)
        color: theme.colorText
        opacity: 0.8
        font { pixelSize: fontSize; preferShaping: false }
        Layout.leftMargin: 0.14 * fontSize
        Layout.minimumWidth: 1.5 * fontSize
    }
}
