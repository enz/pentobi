import QtQuick.Controls 2.2
import "Main.js" as Logic

// We don't use standardButtons because on Android, QtCreator does not
// automatically include the qtbase translations and Dialog in Qt 5.11
// has no mnemonics for the buttons.
DialogButtonBox {
    Button {
        text: Logic.removeShortcut(qsTr("&OK"))
        DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
    }
    Button {
        text: Logic.removeShortcut(qsTr("&Cancel"))
        DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
    }
}
