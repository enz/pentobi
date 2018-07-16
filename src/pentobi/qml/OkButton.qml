import QtQuick.Controls 2.2
import "Controls.js" as PentobiControls

// We don't use standardButtons because on Android, QtCreator does not
// automatically include the qtbase translations and Dialog in Qt 5.11
// has no mnemonics for the buttons.
DialogButtonBox {
    Button {
        text: PentobiControls.addMnemonic(
                  qsTr("OK"),
                  //: Mnemonic for button OK. Leave empty for no mnemonic.
                  qsTr("O"))
        DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
    }
}
