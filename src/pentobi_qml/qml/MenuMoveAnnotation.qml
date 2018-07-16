import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Move Annotation"),
                       //: Mnemonic for menu Move Annotation. Leave empty for no mnemonic.
                       qsTr("A"))
    enabled: gameModel.moveNumber > 0

    Pentobi.MenuItem {
        text: addMnemonic(qsTr("None"),
                          //: Mnemonic for menu item Move Annotation/None. Leave empty for no mnemonic.
                          qsTr("N"))
        checkable: true
        autoExclusive: true
        checked: gameModel.moveAnnotation === ""
        onTriggered: gameModel.moveAnnotation = ""
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Very good"),
                          //: Mnemonic for menu item Very good. Leave empty for no mnemonic.
                          qsTr("V"))
        checkable: true
        autoExclusive: true
        checked: gameModel.moveAnnotation === "!!"
        onTriggered: gameModel.moveAnnotation = "!!"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Good"),
                          //: Mnemonic for menu item Good. Leave empty for no mnemonic.
                          qsTr("G"))
        checkable: true
        autoExclusive: true
        checked: gameModel.moveAnnotation === "!"
        onTriggered: gameModel.moveAnnotation = "!"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Interesting"),
                          //: Mnemonic for menu item Interesting. Leave empty for no mnemonic.
                          qsTr("E"))
        checkable: true
        autoExclusive: true
        checked: gameModel.moveAnnotation === "!?"
        onTriggered: gameModel.moveAnnotation = "!?"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Doubtful"),
                          //: Mnemonic for menu item Doubtful. Leave empty for no mnemonic.
                          qsTr("D"))
        checkable: true
        autoExclusive: true
        checked: gameModel.moveAnnotation === "?!"
        onTriggered: gameModel.moveAnnotation = "?!"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Bad"),
                          //: Mnemonic for menu item Bad. Leave empty for no mnemonic.
                          qsTr("B"))
        checkable: true
        autoExclusive: true
        checked: gameModel.moveAnnotation === "?"
        onTriggered: gameModel.moveAnnotation = "?"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Very Bad"),
                          //: Mnemonic for menu item Very Bad. Leave empty for no mnemonic.
                          qsTr("R"))
        checkable: true
        autoExclusive: true
        checked: gameModel.moveAnnotation === "??"
        onTriggered: gameModel.moveAnnotation = "??"
    }
}
