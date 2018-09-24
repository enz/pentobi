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
    property alias name: textField.text

    function returnPressed() {
        if (! hasButtonFocus())
            checkAccept()
    }
    function checkAccept() {
        if (! isValidName(name))
            return
        if (! gameModel.createFolder(folder + "/" + name)) {
            Logic.showInfo(gameModel.getError())
            return
        }
        accept()
    }

    function isValidName(name) { return name.trim().length > 0 }

    footer: Pentobi.DialogButtonBox {
        Button {
            enabled: isValidName(name)
            text: qsTr("OK")
            onClicked: checkAccept()
        }
        ButtonCancel { }
    }
    onOpened: {
        name = gameModel.suggestNewFolderName(folder)
        textField.selectAll()
    }

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
                onAccepted: checkAccept()
                Layout.fillWidth: true
            }
            Item { Layout.fillWidth: true }
        }
    }
}
