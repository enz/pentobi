//-----------------------------------------------------------------------------
/** @file pentobi/qml/NavigationPanel.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0

ColumnLayout {
    id: root

    function dropCommentFocus() { comment.dropFocus() }

    Comment {
        id: comment

        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Text {
        text: gameModel.positionInfo
        color: theme.colorText
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    }
    NavigationButtons {
        Layout.fillWidth: true
        Layout.maximumHeight: Math.min(43, root.width / 6)
    }
}
