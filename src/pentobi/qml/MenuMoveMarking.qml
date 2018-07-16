import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Move Marking"),
                       //: Mnemonic for menu Move Marking. Leave empty for no mnemonic.
                       qsTr("M"))

    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Last With Dot"),
                          //: Mnemonic for menu item Last With Dot. Leave empty for no mnemonic.
                          qsTr("D"))
        checkable: true
        autoExclusive: true
        checked: gameDisplay.moveMarking !== "last_number"
                 && gameDisplay.moveMarking !== "all_number"
                 && gameDisplay.moveMarking !== "none"
        onTriggered: gameDisplay.moveMarking = "last_dot"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Last With Number"),
                          //: Mnemonic for menu item Last With Number. Leave empty for no mnemonic.
                          qsTr("N"))
        checkable: true
        autoExclusive: true
        checked: gameDisplay.moveMarking === "last_number"
        onTriggered: gameDisplay.moveMarking = "last_number"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("All With Number"),
                          //: Mnemonic for menu item All With Number. Leave empty for no mnemonic.
                          qsTr("A"))
        checkable: true
        autoExclusive: true
        checked: gameDisplay.moveMarking === "all_number"
        onTriggered: gameDisplay.moveMarking = "all_number"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("None"),
                          //: Mnemonic for menu item Move Marking/None. Leave empty for no mnemonic.
                          qsTr("O"))
        checkable: true
        autoExclusive: true
        checked: gameDisplay.moveMarking === "none"
        onTriggered: gameDisplay.moveMarking = "none"
    }
}
