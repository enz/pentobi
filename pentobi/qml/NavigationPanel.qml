//-----------------------------------------------------------------------------
/** @file pentobi/qml/NavigationPanel.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2

ColumnLayout {
    id: root

    property alias comment: comment

    function dropCommentFocus() { comment.dropFocus() }

    Item {
        visible: ! comment.visible
        Layout.fillHeight: true
    }
    Comment {
        id: comment

        Layout.margins: 1
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Label {
        text: gameModel.positionInfo
        color: theme.colorText
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    }
    Item {
        visible: ! comment.visible
        Layout.fillHeight: true
    }
    NavigationButtons {
        Layout.fillWidth: true
        Layout.maximumHeight:
            Math.min(50, 0.08 * rootWindow.contentItem.height, root.width / 6)
    }
}
