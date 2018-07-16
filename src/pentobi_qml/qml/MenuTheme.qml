import "." as Pentobi

Pentobi.Menu {
    title: addMnemonic(qsTr("Theme"),
                       //: Mnemonic for menu Theme. Leave empty for no mnemonic.
                       qsTr("T"))

    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Light"),
                          //: Mnemonic for menu item Theme/Light. Leave empty for no mnemonic.
                          qsTr("L"))
        checkable: true
        autoExclusive: true
        checked: themeName === "light"
        onTriggered: themeName = "light"
    }
    Pentobi.MenuItem {
        text: addMnemonic(qsTr("Dark"),
                          //: Mnemonic for menu item Theme/Dark. Leave empty for no mnemonic.
                          qsTr("D"))
        checkable: true
        autoExclusive: true
        checked: themeName === "dark"
        onTriggered: themeName = "dark"
    }
    Pentobi.MenuItem {
        //: Name of light theme optimized for colorblindness.
        text: addMnemonic(qsTr("Colorblind Light"),
                          //: Mnemonic for menu item Colorblind Light. Leave empty for no mnemonic.
                          qsTr("C"))
        checkable: true
        autoExclusive: true
        checked: themeName === "colorblind-light"
        onTriggered: themeName = "colorblind-light"
    }
    Pentobi.MenuItem {
        //: Name of dark theme optimized for colorblindness.
        text: addMnemonic(qsTr("Colorblind Dark"),
                          //: Mnemonic for menu item Colorblind Dark. Leave empty for no mnemonic.
                          qsTr("A"))
        checkable: true
        autoExclusive: true
        checked: themeName === "colorblind-dark"
        onTriggered: themeName = "colorblind-dark"
    }
}
