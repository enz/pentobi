//-----------------------------------------------------------------------------
/** @file pentobi/qml/NewFolderDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import "." as Pentobi
import "Main.js" as Logic

Pentobi.Dialog {
    id: root

    property url folder

    footer: Pentobi.DialogButtonBox {
        ButtonCancel { }
        ButtonOk { enabled: textField.text.trim().length > 0 }
    }
    onOpened: {
        textField.text = gameModel.suggestNewFolderName(folder)
        textField.selectAll()
    }
    onAccepted: if (! gameModel.createFolder(folder + "/" + textField.text))
                    Logic.showInfo(gameModel.getError())

    Item {
        implicitWidth:
            Math.max(Math.min(rowLayout.implicitWidth, maxContentWidth),
                     minContentWidth)
        implicitHeight: rowLayout.implicitHeight

        ColumnLayout {
            id: rowLayout

            anchors.fill: parent

            Label { text: qsTr("Folder name:") }
            TextField {
                id: textField

                focus: true
                selectByMouse: true
                onAccepted: if (textField.text.trim().length > 0) root.accept()
                Layout.fillWidth: true
            }
            Item { Layout.fillWidth: true }
        }
    }
}
