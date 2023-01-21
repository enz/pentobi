//-----------------------------------------------------------------------------
/** @file pentobi/qml/NewFolderDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Main.js" as Logic

PentobiDialog {
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

    footer: PentobiDialogButtonBox {
        ButtonOk {
            enabled: isValidName(name)
            onClicked: checkAccept()
            DialogButtonBox.buttonRole: DialogButtonBox.InvalidRole
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
