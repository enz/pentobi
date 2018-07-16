import QtQuick 2.0
import QtQuick.Controls 2.2
import "Main.js" as Logic
import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Go"),
                       //: Mnemonic for menu Go. Leave empty for no mnemonic.
                       qsTr("O"))

    Pentobi.MenuItem {
        action: actions.actionGoto
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Go to Move. Leave empty for no mnemonic.
                          qsTr("G"))
    }
    Pentobi.MenuItem {
        action: actions.actionBackToMainVar
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Back to Main Variation. Leave empty for no mnemonic.
                          qsTr("M"))
    }
    Pentobi.MenuItem {
        action: actions.actionBeginningOfBranch
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Beginning Of Branch. Leave empty for no mnemonic.
                          qsTr("B"))
    }
    MenuSeparator { }
    Pentobi.MenuItem {
        action: actions.actionFindNextComment
        text: addMnemonic(action.text,
                          //: Mnemonic for menu item Find Next Comment. Leave empty for no mnemonic.
                          qsTr("C"))
    }
}
